#pragma once
#include <cstdint>
#include "slipstream/codec/md_messages.hpp"
#include "slipstream/marketdata/l1_book.hpp"
#include "slipstream/marketdata/rolling_vwap.hpp"

namespace slipstream::strategy {

enum class Decision : std::uint8_t { NoAction = 0, Buy = 1, Sell = 2, Reject = 3 };

// Tunable risk/trigger parameters sourced from CLI options
struct StrategyConfig {
    std::uint32_t max_quantity;
    std::uint32_t participation_cap_ppm;  // Fraction in parts-per-million (0.1 -> 100'000).
    std::uint32_t band;                   // Hundredths of a bp, as parsed from --band-bps (25.5 -> 2550).
};


class DecisionEngine {
public:
    static constexpr std::uint64_t kMinQuotes = 10;  // Quote half of the warm-up gate.

    DecisionEngine(StrategyConfig config, const slipstream::marketdata::RollingVwap& vwap);

    // Judges one trade against current book/VWAP state. NoAction = still warming up (send nothing);
    // Buy/Sell = accepted; Reject = band/max-qty/participation-cap failed.
    Decision evaluate(const slipstream::codec::Trade& trade, const slipstream::marketdata::L1Book& book,
                      std::uint64_t quote_count) const;

    void on_market_trade(std::uint32_t qty);     // Adds to cumulative market volume (every trade).
    void on_own_fill(std::uint32_t filled_qty);  // Adds to cumulative own qty (every accepted order, for now).

private:
    StrategyConfig config_;
    const slipstream::marketdata::RollingVwap& vwap_;
    std::uint64_t cumulative_market_qty_ = 0;
    std::uint64_t cumulative_own_qty_ = 0;
};

}  // namespace slipstream::strategy
