#include "slipstream/session/heartbeat_monitor.hpp"
#include "slipstream/codec/encoder.hpp"

#include <vector>

namespace slipstream::session {

HeartbeatMonitor::HeartbeatMonitor(slipstream::transport::IFeedTransport& oe_transport)
    : oe_transport_(oe_transport) {
}

void HeartbeatMonitor::on_heartbeat_received(std::uint64_t now_ns) {
    last_seen_ns_ = now_ns;
}

bool HeartbeatMonitor::is_stale(std::uint64_t now_ns) const {
    return (now_ns - last_seen_ns_) > (kStaleTimeoutMs * 1'000'000ULL);
}

void HeartbeatMonitor::send_heartbeat(std::uint64_t now_ns) {
    slipstream::codec::Heartbeat hb{ now_ns };
    std::vector<std::uint8_t> bytes = slipstream::codec::encode_heartbeat(hb);
    oe_transport_.send(bytes.data(), bytes.size());
}

}  // namespace slipstream::session
