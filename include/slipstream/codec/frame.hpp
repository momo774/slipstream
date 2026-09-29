#pragma once
#include <cstdint>
#include <cstddef>

namespace slipstream::codec {

// Message type discriminator carried in every frame header (MD/1 and OE/1).
enum class MsgType : std::uint8_t {
    Quote = 1,
    Trade = 2,
    Heartbeat = 3,
    SessionControl = 4,
    NewOrder = 10,
    ExecReport = 11,
};

#pragma pack(push, 1)
// 4-byte little-endian frame header that precedes every wire message.
struct FrameHeader {
    std::uint16_t body_len;  // Length of body only, header excluded.
    std::uint8_t msg_type;   // See MsgType.
    std::uint8_t version;    // Protocol version (1 for MD/1 and OE/1).
};
static_assert(sizeof(FrameHeader) == 4, "FrameHeader must be exactly 4 bytes");
#pragma pack(pop)

constexpr std::size_t kFrameHeaderSize = sizeof(FrameHeader);  // Bytes consumed by a FrameHeader on the wire.

}  // namespace slipstream::codec
