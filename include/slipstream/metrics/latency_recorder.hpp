#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace slipstream::metrics {

// Records tick-to-order latency samples and reports p50/p99/p99.9 on demand.
class LatencyRecorder {
public:
    explicit LatencyRecorder(std::size_t expected_samples = 1 << 20);  // Reserved up front: no growth in the hot path.

    void record(std::uint64_t tick_ns, std::uint64_t order_sent_ns);

    std::size_t count() const;
    double p50_us() const;
    double p99_us() const;
    double p999_us() const;

private:
    double percentile_us(double p) const;

    std::vector<std::uint64_t> samples_ns_;
};

}  // namespace slipstream::metrics
