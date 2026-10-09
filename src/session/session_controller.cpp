#include "slipstream/session/session_controller.hpp"
#include "slipstream/codec/encoder.hpp"

#include <iostream>
#include <string>
#include <chrono>

namespace slipstream::session {

SessionController::SessionController(slipstream::transport::IFeedTransport& oe_transport)
    : oe_transport_(oe_transport) {
}

void SessionController::poll_stdin() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        return;  // End of input (Ctrl-D or piped stdin): nothing to read.
    }

    if (line == "OPEN") {
        state_ = SessionState::Open;
    } else if (line == "HALT") {
        state_ = SessionState::Halted;
    } else if (line == "CLOSE") {
        state_ = SessionState::Closed;
    } else {
        std::cerr << "Invalid command" << std::endl;
        return;
    }

    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::vector<std::uint8_t> bytes =
        codec::encode_session_control(
                codec::SessionControl{ static_cast<std::uint64_t>(time), static_cast<std::uint8_t>(state_) }
            );
    oe_transport_.send(bytes.data(), bytes.size());
}

SessionState SessionController::current_state() const {
    return state_;
}

}  // namespace slipstream::session
