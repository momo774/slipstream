#include "slipstream/report/execution_report.hpp"

#include <cstdio>

namespace slipstream::report {

void print_execution_report(const ExecutionReport& report) {
    const double executed_pct =
        report.market_qty == 0 ? 0.0 : 100.0 * static_cast<double>(report.executed_qty) / report.market_qty;

    std::printf("=== SLIPSTREAM EXECUTION REPORT ===\n");
    std::printf("%-20s%s\n", "symbol", report.symbol.c_str());
    std::printf("%-20s%llu\n", "market qty", static_cast<unsigned long long>(report.market_qty));
    std::printf("%-20s%llu   (%.4f%%)\n", "executed qty", static_cast<unsigned long long>(report.executed_qty),
                executed_pct);
    std::printf("%-20s%.4f\n", "avg fill price", report.avg_fill_price);
    std::printf("%-20s%.4f\n", "session VWAP", report.session_vwap);
    std::printf("%-20s%.4f bps   (%s)\n", "slippage vs VWAP", report.slippage_bps,
                report.slippage_bps <= 0.0 ? "favorable" : "unfavorable");
    std::printf("%-20s%.4f%%  (cap %.4f%%)\n", "participation", 100.0 * report.participation_rate,
                100.0 * report.participation_cap);
}

}  // namespace slipstream::report
