#include <gtest/gtest.h>
#include "slipstream/strategy/decision_engine.hpp"

using namespace slipstream::codec;
using namespace slipstream::marketdata;
using namespace slipstream::strategy;

namespace {
Trade make_trade(std::uint64_t ts_ms, std::int64_t px, std::uint32_t qty) {
    Trade t{};
    t.ts_ns = ts_ms * 1'000'000;
    t.px = px;
    t.qty = qty;
    return t;
}

struct Fixture {
    RollingVwap vwap{1000};
    L1Book book;
    DecisionEngine engine{{100, 500'000, 2550}, vwap};

    Fixture() {
        vwap.on_trade(make_trade(0, 1'000'000, 10));
        vwap.on_trade(make_trade(1000, 1'000'000, 10));
        engine.on_market_trade(1000);
    }

    Decision eval(std::int64_t bid, std::int64_t ask, std::uint32_t qty = 10, std::uint64_t quotes = 10) {
        Quote q{};
        q.bid_px = bid;
        q.ask_px = ask;
        book.on_quote(q);
        return engine.evaluate(make_trade(1000, 1'000'000, qty), book, quotes);
    }
};
}  // namespace

TEST(DecisionEngineTest, NoActionBeforeWarmUp) {
    Fixture s;
    EXPECT_EQ(s.eval(995'000, 995'000, 10, /*quotes=*/9), Decision::NoAction);
}

TEST(DecisionEngineTest, RejectInsideBand) {
    Fixture s;
    EXPECT_EQ(s.eval(999'900, 1'000'100), Decision::Reject);  // 99.99 / 100.01
}

TEST(DecisionEngineTest, BuyWhenAskBelowBand) {
    Fixture s;
    EXPECT_EQ(s.eval(994'000, 995'000), Decision::Buy);  // ask 99.50
}

TEST(DecisionEngineTest, SellWhenBidAboveBand) {
    Fixture s;
    EXPECT_EQ(s.eval(1'005'000, 1'006'000), Decision::Sell);  // bid 100.50
}

TEST(DecisionEngineTest, RejectOverMaxQuantity) {
    Fixture s;
    EXPECT_EQ(s.eval(994'000, 995'000, /*qty=*/101), Decision::Reject);
}

TEST(DecisionEngineTest, RejectOverParticipationCap) {
    Fixture s;
    s.engine.on_own_fill(495);  // Cumulative: 495 + 10 > 50% of 1000.
    EXPECT_EQ(s.eval(994'000, 995'000), Decision::Reject);
}
