#include "slipstream/codec/encoder.hpp"
#include "slipstream/codec/frame.hpp"
#include "slipstream/codec/byte_order.hpp"

#include <cstdint>

namespace slipstream::codec {

std::vector<std::uint8_t> encode_quote(const Quote& quote) {
    std::vector<std::uint8_t> buffer;
    FrameHeader fh { sizeof(quote), static_cast<uint8_t>(MsgType::Quote), 1};
    write_u16_le(buffer, fh.body_len);
    buffer.push_back(fh.msg_type);
    buffer.push_back(fh.version);
    for (auto i = 0; i < 12; ++i) buffer.push_back(quote.symbol[i]);
    write_u64_le(buffer, quote.ts_ns);
    write_u32_le(buffer, quote.bid_qty);
    write_u64_le(buffer, static_cast<uint64_t>(quote.bid_px));
    write_u32_le(buffer, quote.ask_qty);
    write_u64_le(buffer, static_cast<uint64_t>(quote.ask_px));
    return buffer;
}

std::vector<std::uint8_t> encode_trade(const Trade& trade) {
    std::vector<std::uint8_t> buffer;
    FrameHeader fh { sizeof(trade), static_cast<uint8_t>(MsgType::Trade), 1};
    write_u16_le(buffer, fh.body_len);
    buffer.push_back(fh.msg_type);
    buffer.push_back(fh.version);
    for (auto i = 0; i < 12; ++i) buffer.push_back(trade.symbol[i]);
    write_u64_le(buffer, trade.ts_ns);
    write_u32_le(buffer, trade.qty);
    write_u64_le(buffer, static_cast<uint64_t>(trade.px));
    buffer.push_back(trade.aggressor);
    write_u64_le(buffer, static_cast<uint64_t>(trade.id));
    return buffer;
}

std::vector<std::uint8_t> encode_heartbeat(const Heartbeat& heartbeat) {
    std::vector<std::uint8_t> buffer;
    FrameHeader fh { sizeof(heartbeat), static_cast<uint8_t>(MsgType::Heartbeat), 1};
    write_u16_le(buffer, fh.body_len);
    buffer.push_back(fh.msg_type);
    buffer.push_back(fh.version);
    write_u64_le(buffer, heartbeat.ts_ns);
    return buffer;
}

std::vector<std::uint8_t> encode_session_control(const SessionControl& sc) {
    std::vector<std::uint8_t> buffer;
    FrameHeader fh { sizeof(sc), static_cast<uint8_t>(MsgType::SessionControl), 1};
    write_u16_le(buffer, fh.body_len);
    buffer.push_back(fh.msg_type);
    buffer.push_back(fh.version);
    write_u64_le(buffer, sc.ts_ns);
    buffer.push_back(sc.state);
    return buffer;
}

std::vector<std::uint8_t> encode_new_order(const NewOrder& order) {
    std::vector<std::uint8_t> buffer;
    FrameHeader fh { sizeof(order), static_cast<uint8_t>(MsgType::NewOrder), 1};
    write_u16_le(buffer, fh.body_len);
    buffer.push_back(fh.msg_type);
    buffer.push_back(fh.version);
    write_u64_le(buffer, order.client_order_id);
    for (auto i = 0; i < 12; ++i) buffer.push_back(order.symbol[i]);
    buffer.push_back(order.status);
    write_u64_le(buffer, order.ts_ns);
    write_u64_le(buffer, static_cast<uint64_t>(order.trade_id));
    buffer.push_back(order.side);
    write_u32_le(buffer, order.qty);
    write_u64_le(buffer, static_cast<uint64_t>(order.limit_px));
    return buffer;
}

std::vector<std::uint8_t> encode_exec_report(const ExecReport& report) {
    std::vector<std::uint8_t> buffer;
    FrameHeader fh { sizeof(report), static_cast<uint8_t>(MsgType::ExecReport), 1};
    write_u16_le(buffer, fh.body_len);
    buffer.push_back(fh.msg_type);
    buffer.push_back(fh.version);
    write_u64_le(buffer, report.client_order_id);
    write_u64_le(buffer, report.ts_ns);
    buffer.push_back(report.status);
    write_u32_le(buffer, report.filled_qty);
    write_u64_le(buffer, static_cast<uint64_t>(report.avg_px));
    buffer.push_back(report.reason_code);
    return buffer;
}

}  // namespace slipstream::codec
