#pragma once
#include <cstdint>

namespace slipstream::codec {

#pragma pack(push, 1)
// MD/1 top-of-book quote update for a single symbol. Wire size: 44 bytes.
struct Quote {
    char symbol[12];        // ASCII, null-padded, not null-terminated if full.
    std::uint64_t ts_ns;    // Nanoseconds since Unix epoch.
    std::uint32_t bid_qty;
    std::int64_t bid_px;    // Fixed-point x10,000.
    std::uint32_t ask_qty;
    std::int64_t ask_px;    // Fixed-point x10,000.
};
static_assert(sizeof(Quote) == 44, "Quote must be exactly 44 bytes");

// MD/1 last-trade print for a single symbol. Wire size: 41 bytes.
struct Trade {
    char symbol[12];
    std::uint64_t ts_ns;
    std::uint32_t qty;
    std::int64_t px;        // Fixed-point x10,000.
    char aggressor;         // 'B', 'S', or '?' if unknown.
    std::int64_t id;        // Trade identifier.
};
static_assert(sizeof(Trade) == 41, "Trade must be exactly 41 bytes");

// MD/1 liveness ping, server -> OE client. Wire size: 8 bytes.
struct Heartbeat {
    std::uint64_t ts_ns;  // Server send time.
};
static_assert(sizeof(Heartbeat) == 8, "Heartbeat must be exactly 8 bytes");

// MD/1 session state broadcast, server -> OE client. Wire size: 9 bytes.
struct SessionControl {
    std::uint64_t ts_ns;
    std::uint8_t state;  // 0=OPEN, 1=HALT, 2=CLOSE.
};
static_assert(sizeof(SessionControl) == 9, "SessionControl must be exactly 9 bytes");

#pragma pack(pop)

}  // namespace slipstream::codec
