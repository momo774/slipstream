#pragma once
#include <cstdint>
#include <deque>
#include "slipstream/codec/md_messages.hpp"
#include "slipstream/core/types.hpp"

namespace slipstream::marketdata {

// Rolling VWAP over a trailing time window with O(1) amortized eviction of stale prints.
class RollingVwap {
public:
    explicit RollingVwap(std::uint64_t window_ms);

    void on_trade(const slipstream::codec::Trade& trade);  // Admits a print, evicting anything now stale.
    double value() const;                                  // Current VWAP over the live window, display only.
    bool is_warm() const;                                  // True once enough history has accumulated.

    std::uint64_t sum_qty() const;  // Total qty of prints in the live window.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    __int128 sum_px_qty() const;    // Total px*qty of prints in the live window (fixed-point).
#pragma GCC diagnostic pop

private:
    // One admitted print retained for eviction bookkeeping.
    struct PrintEntry {
        std::uint64_t ts_ns;
        slipstream::core::Price px;
        std::uint32_t qty;
    };

    std::uint64_t window_ms_;
    std::deque<PrintEntry> prints_;  // Prints currently inside the trailing window.

    bool has_trade_ = false;        // Set on the first trade, so first_time_ is meaningful.
    std::uint64_t first_time_ = 0;  // ts_ns of the first trade ever seen (feed start).
    std::uint64_t latest_time = 0; // ts_ns of the most recent trade ("now").

    // __int128 is a GCC/Clang extension (not ISO C++), used deliberately for a 128-bit
    // accumulator for overflow-safety; silenced locally rather than
    // dropping -Wpedantic project-wide.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    __int128 sum_px_qty_ = 0;
#pragma GCC diagnostic pop
    std::uint64_t sum_qty_ = 0;
};

}  // namespace slipstream::marketdata
