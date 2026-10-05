#include <gtest/gtest.h>
#include "slipstream/marketdata/l1_book.hpp"

using namespace slipstream::codec;
using namespace slipstream::marketdata;

namespace {
Quote make_quote(std::int64_t bid_px, std::uint32_t bid_qty, std::int64_t ask_px, std::uint32_t ask_qty) {
    Quote q{};
    q.bid_px = bid_px;
    q.bid_qty = bid_qty;
    q.ask_px = ask_px;
    q.ask_qty = ask_qty;
    return q;
}
}  // namespace

TEST(L1BookTest, EmptyBookIsZero) {
    L1Book book;
    EXPECT_EQ(book.best_bid(), 0);
    EXPECT_EQ(book.best_ask(), 0);
    EXPECT_EQ(book.best_bid_qty(), 0u);
    EXPECT_EQ(book.best_ask_qty(), 0u);
}

TEST(L1BookTest, UpdatesFromQuote) {
    L1Book book;
    book.on_quote(make_quote(1012300, 175, 1012500, 150));

    EXPECT_EQ(book.best_bid(), 1012300);
    EXPECT_EQ(book.best_bid_qty(), 175u);
    EXPECT_EQ(book.best_ask(), 1012500);
    EXPECT_EQ(book.best_ask_qty(), 150u);
}

TEST(L1BookTest, LatestQuoteOverwritesPrevious) {
    L1Book book;
    book.on_quote(make_quote(1012300, 175, 1012500, 150));
    book.on_quote(make_quote(1011000, 20, 1011200, 30));

    EXPECT_EQ(book.best_bid(), 1011000);
    EXPECT_EQ(book.best_bid_qty(), 20u);
    EXPECT_EQ(book.best_ask(), 1011200);
    EXPECT_EQ(book.best_ask_qty(), 30u);
}
