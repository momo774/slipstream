#include <gtest/gtest.h>
#include "slipstream/codec/byte_order.hpp"
#include "slipstream/codec/decoder.hpp"
#include "slipstream/codec/encoder.hpp"
#include "slipstream/codec/frame.hpp"

#include <variant>
#include <vector>

using namespace slipstream::codec;

class StreamReassemblyTest : public ::testing::Test {
protected:
    StreamDecoder decoder;

    void feed(const std::vector<std::uint8_t>& bytes) { decoder.feed(bytes.data(), bytes.size()); }
};

TEST_F(StreamReassemblyTest, EmptyBufferYieldsNothing) {
    EXPECT_FALSE(decoder.try_decode_next().has_value());
}

TEST_F(StreamReassemblyTest, PartialHeaderYieldsNothing) {
    auto bytes = encode_heartbeat(Heartbeat{});
    decoder.feed(bytes.data(), 3);
    EXPECT_FALSE(decoder.try_decode_next().has_value());
}

TEST_F(StreamReassemblyTest, MessageFedOneByteAtATime) {
    Quote q{};
    q.ts_ns = 7;
    auto bytes = encode_quote(q);

    for (std::size_t i = 0; i + 1 < bytes.size(); ++i) {
        decoder.feed(&bytes[i], 1);
        ASSERT_FALSE(decoder.try_decode_next().has_value()) << "decoded early at byte " << i;
    }
    decoder.feed(&bytes.back(), 1);

    auto msg = decoder.try_decode_next();
    ASSERT_TRUE(msg.has_value());
    EXPECT_TRUE(std::holds_alternative<Quote>(*msg));
}

TEST_F(StreamReassemblyTest, TwoMessagesInOneFeedDecodeInOrder) {
    auto bytes = encode_quote(Quote{});
    auto trade = encode_trade(Trade{});
    bytes.insert(bytes.end(), trade.begin(), trade.end());
    feed(bytes);

    auto first = decoder.try_decode_next();
    auto second = decoder.try_decode_next();
    auto third = decoder.try_decode_next();

    ASSERT_TRUE(first.has_value());
    EXPECT_TRUE(std::holds_alternative<Quote>(*first));
    ASSERT_TRUE(second.has_value());
    EXPECT_TRUE(std::holds_alternative<Trade>(*second));
    EXPECT_FALSE(third.has_value());
}

TEST_F(StreamReassemblyTest, MalformedFrameDoesNotStallDecoder) {
    std::vector<std::uint8_t> bad;
    write_u16_le(bad, 8);
    bad.push_back(99);  // unknown msg_type
    bad.push_back(1);
    bad.insert(bad.end(), 8, 0);
    feed(bad);
    EXPECT_FALSE(decoder.try_decode_next().has_value());

    feed(encode_heartbeat(Heartbeat{}));
    auto msg = decoder.try_decode_next();
    ASSERT_TRUE(msg.has_value());
    EXPECT_TRUE(std::holds_alternative<Heartbeat>(*msg));
}
