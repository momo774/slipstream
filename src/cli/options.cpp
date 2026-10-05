#include "slipstream/cli/options.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace slipstream::cli {

Options parse_options(int argc, char** argv) {
    Options opts{};
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (i + 1 >= argc) {
            throw std::invalid_argument("missing value");
        }
        const std::string value = argv[++i];

        if (arg == "--symbol") {
            opts.symbol = value;
        } else if (arg == "--max-quantity") {
            opts.max_quantity = static_cast<std::uint32_t>(std::stoul(value));
        } else if (arg == "--participation-cap") {
            opts.participation_cap = std::stod(value);
        } else if (arg == "--vwap-window-ms") {
            opts.vwap_window_ms = std::stoull(value);
        } else if (arg == "--band-bps") {
            // Stored in hundredths of a bp (25.5 -> 2550) so decision math stays integer-only.
            opts.band_bps = static_cast<std::uint32_t>(std::llround(std::stod(value) * 100));
        } else if (arg == "--transport") {
            opts.transport = value;
        } else if (arg == "--md-host") {
            opts.md_host = value;
        } else if (arg == "--md-port") {
            opts.md_port = static_cast<std::uint16_t>(std::stoul(value));
        } else if (arg == "--oe-host") {
            opts.oe_host = value;
        } else if (arg == "--oe-port") {
            opts.oe_port = static_cast<std::uint16_t>(std::stoul(value));
        } else {
            throw std::invalid_argument("unknown option: " + arg);
        }
    }
    return opts;
}

}  // namespace slipstream::cli
