#pragma once
#include "slipstream/net/socket_fd.hpp"
#include "slipstream/transport/i_feed_transport.hpp"

namespace slipstream::transport {

// Blocking-socket TCP implementation of IFeedTransport
class TcpFeedTransport final : public IFeedTransport {
public:
    explicit TcpFeedTransport(slipstream::net::SocketFd fd);

    std::size_t receive(std::uint8_t* buf, std::size_t len) override;  // recv() wrapper, handles partial reads.
    void send(const std::uint8_t* buf, std::size_t len) override;      // send() wrapper, handles partial writes.
    void close() override;

private:
    slipstream::net::SocketFd fd_;  // Owned connected socket.
};

}  // namespace slipstream::transport
