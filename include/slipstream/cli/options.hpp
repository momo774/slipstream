#pragma once
#include <cstdint>
#include <string>

namespace slipstream::cli {

// Parsed command-line configuration for the slipstream server.
struct Options {
    std::string symbol;
    std::uint32_t max_quantity;
    double participation_cap;
    std::uint64_t vwap_window_ms;
    std::uint32_t band_bps;
    std::string transport;
    std::string md_host;
    std::uint16_t md_port;
    std::string oe_host;
    std::uint16_t oe_port;
};


Options parse_options(int argc, char** argv);

}  // namespace slipstream::cli
