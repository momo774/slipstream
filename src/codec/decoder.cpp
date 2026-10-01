#include "slipstream/codec/decoder.hpp"
#include "slipstream/codec/byte_order.hpp"
#include "slipstream/codec/frame.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>

namespace slipstream::codec {

namespace {
    std::size_t expected_message_size(MsgType msg_type) {
        switch (msg_type) {
            case MsgType::Quote: return sizeof(Quote);
            case MsgType::Trade: return sizeof(Trade);
            case MsgType::Heartbeat: return sizeof(Heartbeat);
            case MsgType::SessionControl: return sizeof(SessionControl);
            case MsgType::NewOrder: return sizeof(NewOrder);
            case MsgType::ExecReport: return sizeof(ExecReport);
            default: return 0;
        }
    }
}

void StreamDecoder::feed(const std::uint8_t* data, std::size_t len) {
    for (std::size_t i{0}; i < len; ++i) {
        this->buffer_.push_back(data[i]);
    }
}

// handle message split across multiple feed() calls
std::optional<DecodedMessage> StreamDecoder::try_decode_next() {
    if (this->buffer_.size() < std::size_t(4)) {
        return std::nullopt;
    }
    std::size_t offset{0};
    std::uint16_t msg_size = read_u16_le(this->buffer_, offset);
    offset += 2;
    std::uint8_t msg_type_num = this->buffer_[offset];
    offset += 2; // skip version
    if (this->buffer_.size() < static_cast<std::size_t>(msg_size) + 4) {
        return std::nullopt;
    }
    MsgType msg_type{static_cast<MsgType>(msg_type_num)};
    if (expected_message_size(msg_type) != msg_size) {
        return std::nullopt;
    }
    if (msg_type == MsgType::Quote) {
        Quote q{};
        std::memcpy(&q, this->buffer_.data() + 4, msg_size);
        return q;
    }
    if (msg_type == MsgType::Trade) {
        Trade t{};
        std::memcpy(&t, this->buffer_.data() + 4, msg_size);
        return t;
    }
    if (msg_type == MsgType::Heartbeat) {
        Heartbeat h{};
        std::memcpy(&h, this->buffer_.data() + 4, msg_size);
        return h;
    }
    if (msg_type == MsgType::SessionControl) {
        SessionControl sc{};
        std::memcpy(&sc, this->buffer_.data() + 4, msg_size);
        return sc;
    }
    if (msg_type == MsgType::NewOrder) {
        NewOrder n{};
        std::memcpy(&n, this->buffer_.data() + 4, msg_size);
        return n;
    }
    if (msg_type == MsgType::ExecReport) {
        ExecReport e{};
        std::memcpy(&e, this->buffer_.data() + 4, msg_size);
        return e;
    }
    this->buffer_.erase(this->buffer_.begin(), this->buffer_.end());
    // should never reach
    return std::nullopt;
}
}  // namespace slipstream::codec



