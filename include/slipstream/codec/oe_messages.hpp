#pragma once
#include <cstdint>

namespace slipstream::codec {

#pragma pack(push, 1)
// OE/1 order sent from server to the OE client. Wire size: 42 bytes, msg_type=10.
struct NewOrder {
    std::uint64_t client_order_id;  // Strictly increasing, unique per process run.
    char symbol[12];
    char status;                    // 'A' ACCEPTED, 'R' REJECTED.
    std::uint64_t ts_ns;
    std::int64_t trade_id;          // The trade that generated this order.
    char side;                      // 'B' or 'S'.
    std::uint32_t qty;
    std::int64_t limit_px;          // Fixed-point x10,000.
};

// OE/1 execution report sent from the OE client back to the server (optional). Wire size: 30 bytes, msg_type=11.
struct ExecReport {
    std::uint64_t client_order_id;  // Echo of the order this refers to.
    std::uint64_t ts_ns;            // Gateway timestamp.
    std::uint8_t status;            // 0=ACK, 1=FILL, 2=PARTIAL, 3=REJECT.
    std::uint32_t filled_qty;       // Cumulative for this order.
    std::int64_t avg_px;            // Fixed-point x10,000.
    std::uint8_t reason_code;       // 0=none, 1=risk, 2=price, 3=size, 4=throttle.
};
#pragma pack(pop)

}  // namespace slipstream::codec
