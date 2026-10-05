#include <gtest/gtest.h>
#include "slipstream/marketdata/rolling_vwap.hpp"

using namespace slipstream::codec;
using namespace slipstream::marketdata;

namespace {
constexpr std::uint64_t kWindowMs = 1000;

// Prices are fixed-point x10,000, so 1,000,000 == 100.00
Trade make_trade(std::uint64_t ts_ms, std::int64_t px, std::uint32_t qty) {
    Trade t{};
    t.ts_ns = ts_ms * 1'000'000;
    t.px = px;
    t.qty = qty;
    return t;
}
}  // namespace

TEST(RollingVwapTest, EmptyWindowHasNoVolume) {
    RollingVwap vwap(kWindowMs);
    EXPECT_EQ(vwap.sum_qty(), 0u);
    EXPECT_DOUBLE_EQ(vwap.value(), 0.0);  // No divide-by-zero on an empty window.
}

TEST(RollingVwapTest, SingleTrade) {
    RollingVwap vwap(kWindowMs);
    vwap.on_trade(make_trade(0, 1'000'000, 5));
    EXPECT_EQ(vwap.sum_qty(), 5u);
    EXPECT_DOUBLE_EQ(vwap.value(), 100.0);
}

TEST(RollingVwapTest, WeightsByQuantity) {
    RollingVwap vwap(kWindowMs);
    vwap.on_trade(make_trade(0, 1'000'000, 1));    // 100.00 x 1
    vwap.on_trade(make_trade(10, 1'030'000, 2));   // 103.00 x 2
    EXPECT_DOUBLE_EQ(vwap.value(), 102.0);         // (100 + 206) / 3
}

TEST(RollingVwapTest, SlidingNotTumbling) {
    RollingVwap vwap(kWindowMs);
    vwap.on_trade(make_trade(0, 1'000'000, 1));     // 100
    vwap.on_trade(make_trade(600, 1'100'000, 1));   // 110
    EXPECT_DOUBLE_EQ(vwap.value(), 105.0);

    // At t=1200 the window is (200, 1200]: only the t=0 print falls out.
    // A tumbling window would have reset at t=1000 and report 120.
    vwap.on_trade(make_trade(1200, 1'200'000, 1));  // 120
    EXPECT_EQ(vwap.sum_qty(), 2u);
    EXPECT_DOUBLE_EQ(vwap.value(), 115.0);

    // At t=1700 the window is (700, 1700]: the t=600 print falls out too.
    vwap.on_trade(make_trade(1700, 1'300'000, 1));  // 130
    EXPECT_EQ(vwap.sum_qty(), 2u);
    EXPECT_DOUBLE_EQ(vwap.value(), 125.0);
}

TEST(RollingVwapTest, LongGapEvictsEverythingStale) {
    RollingVwap vwap(kWindowMs);
    vwap.on_trade(make_trade(0, 1'000'000, 1));
    vwap.on_trade(make_trade(100, 1'000'000, 1));
    vwap.on_trade(make_trade(200, 1'000'000, 1));

    vwap.on_trade(make_trade(5000, 1'500'000, 4));
    EXPECT_EQ(vwap.sum_qty(), 4u);
    EXPECT_DOUBLE_EQ(vwap.value(), 150.0);
}

TEST(RollingVwapTest, SameNanosecondBurst) {
    RollingVwap vwap(kWindowMs);
    for (int i = 0; i < 5; ++i) {
        vwap.on_trade(make_trade(0, 1'000'000, 1));
    }
    EXPECT_EQ(vwap.sum_qty(), 5u);  // Same timestamp as "now" -> none evicted.

    vwap.on_trade(make_trade(2000, 1'200'000, 1));
    EXPECT_EQ(vwap.sum_qty(), 1u);  // Whole burst evicted together.
    EXPECT_DOUBLE_EQ(vwap.value(), 120.0);
}

TEST(RollingVwapTest, LargeNotionalDoesNotOverflow) {
    RollingVwap vwap(kWindowMs);
    // 1e14 (fixed-point) x 1e6 = 1e20, past int64's ~9.2e18 -> needs the __int128 widening.
    vwap.on_trade(make_trade(0, 100'000'000'000'000, 1'000'000));
    vwap.on_trade(make_trade(1, 100'000'000'000'000, 1'000'000));
    EXPECT_DOUBLE_EQ(vwap.value(), 1e10);
}

TEST(RollingVwapTest, NotWarmBeforeFullWindow) {
    RollingVwap vwap(kWindowMs);
    vwap.on_trade(make_trade(0, 1'000'000, 1));
    vwap.on_trade(make_trade(500, 1'000'000, 1));
    EXPECT_FALSE(vwap.is_warm());
}
