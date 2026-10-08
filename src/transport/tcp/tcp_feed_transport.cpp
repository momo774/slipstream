#include "tcp_feed_transport.hpp"
#include "slipstream/transport/tcp_feed_transport.hpp"

#include <utility>
#include <sys/socket.h>
#include <cerrno>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

namespace {
    void check(int rc, const char* what) {
        if (rc < 0) {
            throw std::system_error(errno, std::generic_category(), what);
        }
    }
}

namespace slipstream::transport {

TcpFeedTransport::TcpFeedTransport(slipstream::net::SocketFd fd) : fd_(std::move(fd)) {
}

std::size_t TcpFeedTransport::receive(std::uint8_t* buffer, std::size_t len) {
    while (true) {
        ssize_t bytes_received = ::recv(fd_.get(), buffer, len, 0);
        if (bytes_received >= 0) {
            return static_cast<std::size_t>(bytes_received);
        }
        if (bytes_received < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category(), "recv");
        }
    }
}

void TcpFeedTransport::send(const std::uint8_t *buffer, std::size_t len) {
    std::size_t bytes_sent = 0;
    while (bytes_sent < len) {
        ssize_t bytes = ::send(fd_.get(), buffer + bytes_sent, len - bytes_sent, MSG_NOSIGNAL);
        if (bytes < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category(), "send");
        }
        bytes_sent += static_cast<std::size_t>(bytes);
    }
}

void TcpFeedTransport::close() {
    if (fd_.valid()) {
        ::shutdown(fd_.get(), SHUT_RDWR);
    }
    fd_ = slipstream::net::SocketFd{}; // calls move assignment and closes it
}

int TcpFeedTransport::fd() const {
    return fd_.get();
}

std::unique_ptr<IFeedTransport> accept_tcp_feed_transport(const std::string& host, std::uint16_t port) {
    int one{1};
    int raw_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    check(raw_fd, "socket");
    slipstream::net::SocketFd fd{raw_fd};
    check(::setsockopt(fd.get(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)), "setsockopt");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // inet_pton returns 1 on success, 0 for an unparseable host (errno is not set in that case).
    if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
        throw std::invalid_argument("invalid IPv4 host: " + host);
    }

    check(::bind(fd.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)), "bind");
    check(::listen(fd.get(), 1), "listen");
    slipstream::net::SocketFd clientfd;
    while (true) {
        int raw_client_fd = ::accept(fd.get(), nullptr, nullptr);
        if (raw_client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            check(raw_client_fd, "accept");
        }
        clientfd = slipstream::net::SocketFd{raw_client_fd};
        break;
    }

    check(::setsockopt(clientfd.get(), IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one)), "clientfd-setsockopt");
    return std::make_unique<TcpFeedTransport>(std::move(clientfd));
}

}  // namespace slipstream::transport
