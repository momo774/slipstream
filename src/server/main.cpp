#include <cstdint>
#include <exception>
#include <iostream>

#include <cmath>

#include "event_loop.hpp"
#include "md_server.hpp"
#include "oe_server.hpp"
#include "slipstream/cli/options.hpp"
#include "slipstream/marketdata/l1_book.hpp"
#include "slipstream/marketdata/rolling_vwap.hpp"
#include "slipstream/report/execution_report.hpp"
#include "slipstream/strategy/decision_engine.hpp"

int main(int argc, char** argv) {
    using namespace slipstream;

    try {
        const cli::Options options = cli::parse_options(argc, argv);
        marketdata::L1Book book;
        std::uint64_t quote_count = 0;

        marketdata::RollingVwap vwap(options.vwap_window_ms);
        const strategy::StrategyConfig config{
            options.max_quantity,
            static_cast<std::uint32_t>(std::llround(options.participation_cap * 1'000'000)),
            options.band_bps,
        };
        strategy::DecisionEngine engine(config, vwap);

        server::MdServer md_server(options, book, quote_count);
        server::OeServer oe_server(options, book, quote_count, vwap, engine);

        std::cout << "waiting for MD client on " << options.md_host << ':' << options.md_port << '\n';
        md_server.accept();
        std::cout << "waiting for OE client on " << options.oe_host << ':' << options.oe_port << '\n';
        oe_server.accept();

        server::run_event_loop(md_server, oe_server);
        report::print_execution_report(oe_server.build_report());
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
