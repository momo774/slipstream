#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "slipstream/transport/i_feed_transport.hpp"

namespace slipstream::transport {

// Listens on host:port, blocks until one client connects, and returns a blocking TCP transport for it.
std::unique_ptr<IFeedTransport> accept_tcp_feed_transport(const std::string& host, std::uint16_t port);

}  // namespace slipstream::transport
