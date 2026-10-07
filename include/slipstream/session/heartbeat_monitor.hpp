#pragma once
#include <cstdint>
#include "slipstream/transport/i_feed_transport.hpp"

namespace slipstream::session {

// Detects OE-client heartbeat staleness (5s timeout) and sends heartbeats to the OE client.
class HeartbeatMonitor {
public:
    static constexpr std::uint32_t kStaleTimeoutMs = 5000;

    explicit HeartbeatMonitor(slipstream::transport::IFeedTransport& oe_transport);

    void on_heartbeat_received(std::uint64_t now_ns);
    bool is_stale(std::uint64_t now_ns) const;
    void send_heartbeat(std::uint64_t now_ns);

private:
    slipstream::transport::IFeedTransport& oe_transport_;
    std::uint64_t last_seen_ns_ = 0;
};

}  // namespace slipstream::session
