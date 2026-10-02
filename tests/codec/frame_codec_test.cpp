#include <gtest/gtest.h>
#include "slipstream/codec/byte_order.hpp"
#include "slipstream/codec/encoder.hpp"
#include "slipstream/codec/frame.hpp"

#include <cstdint>

using namespace slipstream::codec;

TEST(FrameCodecTest, HeaderFieldsAreInWireOrder) {
    Heartbeat h{};
    h.ts_ns = 42;
    auto bytes = encode_heartbeat(h);

    ASSERT_EQ(bytes.size(), kFrameHeaderSize + sizeof(Heartbeat));
    EXPECT_EQ(read_u16_le(bytes, 0), sizeof(Heartbeat));
    EXPECT_EQ(bytes[2], static_cast<std::uint8_t>(MsgType::Heartbeat));
    EXPECT_EQ(bytes[3], 1);
}

TEST(FrameCodecTest, EveryEncoderEmitsHeaderPlusBody) {
    EXPECT_EQ(encode_quote(Quote{}).size(), kFrameHeaderSize + sizeof(Quote));
    EXPECT_EQ(encode_trade(Trade{}).size(), kFrameHeaderSize + sizeof(Trade));
    EXPECT_EQ(encode_heartbeat(Heartbeat{}).size(), kFrameHeaderSize + sizeof(Heartbeat));
    EXPECT_EQ(encode_session_control(SessionControl{}).size(), kFrameHeaderSize + sizeof(SessionControl));
    EXPECT_EQ(encode_new_order(NewOrder{}).size(), kFrameHeaderSize + sizeof(NewOrder));
    EXPECT_EQ(encode_exec_report(ExecReport{}).size(), kFrameHeaderSize + sizeof(ExecReport));
}
