#pragma once
#include <cstdint>
#include "slipstream/transport/i_feed_transport.hpp"

namespace slipstream::session {

enum class SessionState : std::uint8_t { Open = 0, Halted = 1, Closed = 2 };

// Reads HALT/OPEN/CLOSE from console stdin and pushes SessionControl messages to the OE client.
class SessionController {
public:
    explicit SessionController(slipstream::transport::IFeedTransport& oe_transport);

    void poll_stdin();  // Non-blocking check for a pending operator command line.
    SessionState current_state() const;

private:
    slipstream::transport::IFeedTransport& oe_transport_;
    SessionState state_ = SessionState::Open;
};

}  // namespace slipstream::session
