#include <cstdint>
#include <exception>
#include <iostream>

#include "md_server.hpp"
#include "slipstream/cli/options.hpp"
#include "slipstream/core/fixed_point.hpp"
#include "slipstream/marketdata/l1_book.hpp"

int main(int argc, char** argv) {
    using namespace slipstream;

    try {
        const cli::Options options = cli::parse_options(argc, argv);

        marketdata::L1Book book;
        std::uint64_t quote_count = 0;

        server::MdServer md_server(options, book, quote_count);
        std::cout << "waiting for MD client on " << options.md_host << ':' << options.md_port << '\n';
        md_server.run();

        std::cout << "MD client disconnected. " << options.symbol << " quotes: " << quote_count
                  << ", best bid " << core::to_display_price(book.best_bid())
                  << " x " << book.best_bid_qty()
                  << ", best ask " << core::to_display_price(book.best_ask())
                  << " x " << book.best_ask_qty() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
