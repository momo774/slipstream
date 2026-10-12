#include "slipstream/metrics/latency_recorder.hpp"

#include <algorithm>

namespace slipstream::metrics {

LatencyRecorder::LatencyRecorder(std::size_t expected_samples) {
    samples_ns_.reserve(expected_samples);
}

void LatencyRecorder::record(std::uint64_t tick_ns, std::uint64_t order_sent_ns) {
    samples_ns_.push_back(order_sent_ns - tick_ns);
}

std::size_t LatencyRecorder::count() const {
    return samples_ns_.size();
}

double LatencyRecorder::percentile_us(double p) const {
    if (samples_ns_.empty()) {
        return 0.0;
    }
    std::vector<std::uint64_t> sorted = samples_ns_;
    std::sort(sorted.begin(), sorted.end());
    const auto index = static_cast<std::size_t>(p * static_cast<double>(sorted.size() - 1));
    return static_cast<double>(sorted[index]) / 1000.0;
}

double LatencyRecorder::p50_us() const {
    return percentile_us(0.50);
}

double LatencyRecorder::p99_us() const {
    return percentile_us(0.99);
}

double LatencyRecorder::p999_us() const {
    return percentile_us(0.999);
}

}  // namespace slipstream::metrics
