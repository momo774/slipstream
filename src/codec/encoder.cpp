#include "slipstream/codec/encoder.hpp"
#include "slipstream/codec/frame.hpp"

#include <cstring>

using FrameType = slipstream::codec::MsgType;

namespace slipstream::codec {

std::vector<std::uint8_t> encode_quote(const Quote& quote) {
    std::vector<std::uint8_t> frame;
    FrameHeader fh { sizeof(quote), static_cast<uint8_t>(FrameType::Quote), 1};
    std::memcpy(frame.data(), &fh, kFrameHeaderSize);
    std::memcpy(frame.data() + kFrameHeaderSize, &quote, sizeof(quote));
    return frame;
}

std::vector<std::uint8_t> encode_trade(const Trade& trade) {
    std::vector<std::uint8_t> frame;
    FrameHeader fh { sizeof(trade), static_cast<uint8_t>(FrameType::Trade), 1};
    std::memcpy(frame.data(), &fh, kFrameHeaderSize);
    std::memcpy(frame.data() + kFrameHeaderSize, &trade, sizeof(trade));
    return frame;
}

std::vector<std::uint8_t> encode_heartbeat(const Heartbeat& heartbeat) {
    std::vector<std::uint8_t> frame;
    FrameHeader fh { sizeof(heartbeat), static_cast<uint8_t>(FrameType::Heartbeat), 1};
    std::memcpy(frame.data(), &fh, kFrameHeaderSize);
    std::memcpy(frame.data() + kFrameHeaderSize, &heartbeat, sizeof(heartbeat));
    return frame;
}

std::vector<std::uint8_t> encode_session_control(const SessionControl& sc) {
    std::vector<std::uint8_t> frame;
    FrameHeader fh { sizeof(sc), static_cast<uint8_t>(FrameType::SessionControl), 1};
    std::memcpy(frame.data(), &fh, kFrameHeaderSize);
    std::memcpy(frame.data() + kFrameHeaderSize, &sc, sizeof(sc));
    return frame;
}

std::vector<std::uint8_t> encode_new_order(const NewOrder& order) {
    std::vector<std::uint8_t> frame;
    FrameHeader fh { sizeof(order), static_cast<uint8_t>(FrameType::NewOrder), 1};
    std::memcpy(frame.data(), &fh, kFrameHeaderSize);
    std::memcpy(frame.data() + kFrameHeaderSize, &order, sizeof(order));
    return frame;
}

std::vector<std::uint8_t> encode_exec_report(const ExecReport& report) {
    std::vector<std::uint8_t> frame;
    FrameHeader fh { sizeof(report), static_cast<uint8_t>(FrameType::ExecReport), 1};
    std::memcpy(frame.data(), &fh, kFrameHeaderSize);
    std::memcpy(frame.data() + kFrameHeaderSize, &report, sizeof(report));
    return frame;
}

}  // namespace slipstream::codec
