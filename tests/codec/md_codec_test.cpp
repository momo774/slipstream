#include <gtest/gtest.h>
#include "slipstream/codec/decoder.hpp"
#include "slipstream/codec/encoder.hpp"

#include <cstring>
#include <optional>
#include <variant>
#include <vector>

using namespace slipstream::codec;

namespace {
std::optional<DecodedMessage> roundtrip(const std::vector<std::uint8_t>& bytes) {
    StreamDecoder d;
    d.feed(bytes.data(), bytes.size());
    return d.try_decode_next();
}
}  // namespace

// Packed fields can't bind to EXPECT_EQ's references, so `+field` copies them first.

TEST(MdCodecTest, QuoteRoundTrip) {
    Quote in{};
    std::memcpy(in.symbol, "SYNTH1", 6);
    in.ts_ns = 1'700'000'000'000'000'000ULL;
    in.bid_qty = 175;
    in.bid_px = 1012300;
    in.ask_qty = 150;
    in.ask_px = 1012500;

    auto msg = roundtrip(encode_quote(in));
    ASSERT_TRUE(msg.has_value());
    ASSERT_TRUE(std::holds_alternative<Quote>(*msg));
    const Quote& out = std::get<Quote>(*msg);

    EXPECT_EQ(std::memcmp(out.symbol, in.symbol, 12), 0);
    EXPECT_EQ(+out.ts_ns, +in.ts_ns);
    EXPECT_EQ(+out.bid_qty, +in.bid_qty);
    EXPECT_EQ(+out.bid_px, +in.bid_px);
    EXPECT_EQ(+out.ask_qty, +in.ask_qty);
    EXPECT_EQ(+out.ask_px, +in.ask_px);
}

TEST(MdCodecTest, TradeRoundTrip) {
    Trade in{};
    std::memcpy(in.symbol, "SYNTH2", 6);
    in.ts_ns = 1'700'000'000'500'000'000ULL;
    in.qty = 65;
    in.px = 2485300;
    in.aggressor = 'B';
    in.id = 987654321;

    auto msg = roundtrip(encode_trade(in));
    ASSERT_TRUE(msg.has_value());
    ASSERT_TRUE(std::holds_alternative<Trade>(*msg));
    const Trade& out = std::get<Trade>(*msg);

    EXPECT_EQ(std::memcmp(out.symbol, in.symbol, 12), 0);
    EXPECT_EQ(+out.ts_ns, +in.ts_ns);
    EXPECT_EQ(+out.qty, +in.qty);
    EXPECT_EQ(+out.px, +in.px);
    EXPECT_EQ(+out.aggressor, +in.aggressor);
    EXPECT_EQ(+out.id, +in.id);
}

TEST(MdCodecTest, HeartbeatRoundTrip) {
    Heartbeat in{};
    in.ts_ns = 123456789;

    auto msg = roundtrip(encode_heartbeat(in));
    ASSERT_TRUE(msg.has_value());
    ASSERT_TRUE(std::holds_alternative<Heartbeat>(*msg));
    EXPECT_EQ(+std::get<Heartbeat>(*msg).ts_ns, +in.ts_ns);
}

TEST(MdCodecTest, SessionControlRoundTrip) {
    SessionControl in{};
    in.ts_ns = 987654321;
    in.state = 1;

    auto msg = roundtrip(encode_session_control(in));
    ASSERT_TRUE(msg.has_value());
    ASSERT_TRUE(std::holds_alternative<SessionControl>(*msg));
    const SessionControl& out = std::get<SessionControl>(*msg);

    EXPECT_EQ(+out.ts_ns, +in.ts_ns);
    EXPECT_EQ(+out.state, +in.state);
}
