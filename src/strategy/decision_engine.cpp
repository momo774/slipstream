#include "slipstream/strategy/decision_engine.hpp"

#include <cstdint>

namespace slipstream::strategy {

DecisionEngine::DecisionEngine(StrategyConfig config, const slipstream::marketdata::RollingVwap& vwap)
    : config_(config), vwap_(vwap) {
}

// All comparisons are integer cross-multiplications in __int128, so no overflow.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
Decision DecisionEngine::evaluate(const slipstream::codec::Trade& trade,
                                  const slipstream::marketdata::L1Book& book,
                                  std::uint64_t quote_count) const {
    using i128 = __int128;
    constexpr i128 kScale = 1'000'000;  // band is hundredths of a bp, cap is ppm: both are x/1'000'000.

    // 1. Warm-up: full VWAP window and enough quotes, otherwise send nothing.
    if (!vwap_.is_warm() || quote_count < kMinQuotes) {
        return Decision::NoAction;
    }

    // 2. Band trigger. VWAP = sum_px_qty / sum_qty, so multiply through by sum_qty instead of dividing.
    const i128 sum_px_qty = vwap_.sum_px_qty();
    const i128 sum_qty = vwap_.sum_qty();
    const i128 ask = book.best_ask();
    const i128 bid = book.best_bid();

    Decision side;
    if (ask * sum_qty * kScale <= sum_px_qty * (kScale - config_.band)) {
        side = Decision::Buy;
    } else if (bid * sum_qty * kScale >= sum_px_qty * (kScale + config_.band)) {
        side = Decision::Sell;
    } else {
        return Decision::Reject;
    }

    // 3. Max quantity per order.
    const std::uint32_t qty = trade.qty;
    if (qty > config_.max_quantity) {
        return Decision::Reject;
    }

    // 4. Participation cap on cumulative volume: (own + qty) / market <= cap_ppm / 1'000'000.
    const i128 own_after = static_cast<i128>(cumulative_own_qty_) + qty;
    if (own_after * kScale > static_cast<i128>(config_.participation_cap_ppm) * cumulative_market_qty_) {
        return Decision::Reject;
    }

    return side;
}
#pragma GCC diagnostic pop

void DecisionEngine::on_market_trade(std::uint32_t qty) {
    this->cumulative_market_qty_ += static_cast<std::uint64_t>(qty);
}

void DecisionEngine::on_own_fill(std::uint32_t filled_qty) {
    this->cumulative_own_qty_ += static_cast<std::uint64_t>(filled_qty);
}

}  // namespace slipstream::strategy
