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

TEST(OeCodecTest, NewOrderRoundTrip) {
    NewOrder in{};
    in.client_order_id = 42;
    std::memcpy(in.symbol, "SYNTH1", 6);
    in.status = 'A';
    in.ts_ns = 1'700'000'000'000'000'000ULL;
    in.trade_id = 555;
    in.side = 'S';
    in.qty = 300;
    in.limit_px = 1012400;

    auto msg = roundtrip(encode_new_order(in));
    ASSERT_TRUE(msg.has_value());
    ASSERT_TRUE(std::holds_alternative<NewOrder>(*msg));
    const NewOrder& out = std::get<NewOrder>(*msg);

    EXPECT_EQ(+out.client_order_id, +in.client_order_id);
    EXPECT_EQ(std::memcmp(out.symbol, in.symbol, 12), 0);
    EXPECT_EQ(+out.status, +in.status);
    EXPECT_EQ(+out.ts_ns, +in.ts_ns);
    EXPECT_EQ(+out.trade_id, +in.trade_id);
    EXPECT_EQ(+out.side, +in.side);
    EXPECT_EQ(+out.qty, +in.qty);
    EXPECT_EQ(+out.limit_px, +in.limit_px);
}

TEST(OeCodecTest, ExecReportRoundTrip) {
    ExecReport in{};
    in.client_order_id = 42;
    in.ts_ns = 1'700'000'000'250'000'000ULL;
    in.status = 2;
    in.filled_qty = 120;
    in.avg_px = 1012450;
    in.reason_code = 0;

    auto msg = roundtrip(encode_exec_report(in));
    ASSERT_TRUE(msg.has_value());
    ASSERT_TRUE(std::holds_alternative<ExecReport>(*msg));
    const ExecReport& out = std::get<ExecReport>(*msg);

    EXPECT_EQ(+out.client_order_id, +in.client_order_id);
    EXPECT_EQ(+out.ts_ns, +in.ts_ns);
    EXPECT_EQ(+out.status, +in.status);
    EXPECT_EQ(+out.filled_qty, +in.filled_qty);
    EXPECT_EQ(+out.avg_px, +in.avg_px);
    EXPECT_EQ(+out.reason_code, +in.reason_code);
}
