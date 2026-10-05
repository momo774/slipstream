#include "slipstream/marketdata/l1_book.hpp"

#include "slipstream/codec/encoder.hpp"

namespace slipstream::marketdata {

void L1Book::on_quote(const slipstream::codec::Quote& quote) {
    bid_px_ = quote.bid_px;
    bid_qty_ = quote.bid_qty;
    ask_px_ = quote.ask_px;
    ask_qty_ = quote.ask_qty;
}

slipstream::core::Price L1Book::best_bid() const {
    return bid_px_;
}

slipstream::core::Price L1Book::best_ask() const {
    return ask_px_;
}

std::uint32_t L1Book::best_bid_qty() const {
    return bid_qty_;
}

std::uint32_t L1Book::best_ask_qty() const {
    return ask_qty_;
}

}  // namespace slipstream::marketdata
