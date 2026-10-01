#pragma once
#include "slipstream/codec/md_messages.hpp"
#include "slipstream/codec/oe_messages.hpp"

#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

namespace slipstream::codec {

using DecodedMessage = std::variant<Quote, Trade, Heartbeat, SessionControl, NewOrder, ExecReport>;

// Buffers partially-received bytes across recv() calls and yields complete decoded messages.
class StreamDecoder {
public:
    void feed(const std::uint8_t* data, std::size_t len);  // Appends freshly-received bytes.
    std::optional<DecodedMessage> try_decode_next();        // Pops the next complete message, if any.

private:
    std::vector<std::uint8_t> buffer_;
};

}  // namespace slipstream::codec
