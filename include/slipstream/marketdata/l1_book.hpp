#pragma once
#include <cstdint>
#include "slipstream/codec/md_messages.hpp"
#include "slipstream/core/types.hpp"

namespace slipstream::marketdata {

// Top-of-book (L1) state for a single traded symbol.
class L1Book {
public:
    void on_quote(const slipstream::codec::Quote& quote);  // Updates best bid/ask from an incoming quote.

    slipstream::core::Price best_bid() const;
    slipstream::core::Price best_ask() const;
    std::uint32_t best_bid_qty() const;
    std::uint32_t best_ask_qty() const;

private:
    slipstream::core::Price bid_px_ = 0;
    slipstream::core::Price ask_px_ = 0;
    std::uint32_t bid_qty_ = 0;
    std::uint32_t ask_qty_ = 0;
};

}  // namespace slipstream::marketdata
