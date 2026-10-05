#include "slipstream/marketdata/rolling_vwap.hpp"

#include "slipstream/core/fixed_point.hpp"

namespace slipstream::marketdata {

RollingVwap::RollingVwap(std::uint64_t window_ms) : window_ms_(window_ms) {
}

// Must be a sliding window (evict one at a time, relative to "now")
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
void RollingVwap::on_trade(const slipstream::codec::Trade& trade) {
    const std::uint64_t window_ns = window_ms_ * 1'000'000;
    if (!has_trade_) {
        first_time_ = trade.ts_ns;
        has_trade_ = true;
    }
    latest_time = trade.ts_ns;
    sum_px_qty_ += static_cast<__int128>(trade.px) * trade.qty; // overflows so needs cast to 128bits
    sum_qty_ += trade.qty;
    prints_.push_back({ latest_time, trade.px, trade.qty });

    while (!prints_.empty() && (prints_.front().ts_ns + window_ns < latest_time)) {
        sum_px_qty_ -= static_cast<__int128>(prints_.front().px) * prints_.front().qty;
        sum_qty_ -= prints_.front().qty;
        prints_.pop_front();
    }
}
#pragma GCC diagnostic pop

double RollingVwap::value() const {
    if (sum_qty_ == 0) return 0.0;
    return static_cast<double>(sum_px_qty_) / core::kPriceScale / static_cast<double>(sum_qty_) ;
}

bool RollingVwap::is_warm() const {
    return has_trade_ && latest_time - first_time_ >= window_ms_ * 1'000'000;
}

std::uint64_t RollingVwap::sum_qty() const {
    return sum_qty_;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
__int128 RollingVwap::sum_px_qty() const {
    return sum_px_qty_;
}
#pragma GCC diagnostic pop

}  // namespace slipstream::marketdata
