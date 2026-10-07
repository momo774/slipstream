#pragma once
#include <cstdint>
#include <string>

namespace slipstream::report {

// Final run summary printed to stdout on server exit.
struct ExecutionReport {
    std::string symbol;
    std::uint64_t market_qty;
    std::uint64_t executed_qty;
    double avg_fill_price;
    double session_vwap;
    double slippage_bps;
    double participation_rate;
    double participation_cap;
};

void print_execution_report(const ExecutionReport& report);

}  // namespace slipstream::report
