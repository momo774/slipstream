#include "slipstream/net/socket_fd.hpp"

#include <unistd.h>

namespace slipstream::net {

SocketFd::SocketFd(int fd) : fd_(fd) {
}

SocketFd::~SocketFd() {
    if (this->valid()) {
        close(fd_);
    }
}

SocketFd::SocketFd(SocketFd&& other) noexcept {
    fd_ = other.release();
}

SocketFd& SocketFd::operator=(SocketFd&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    if (this->valid()) {
        close(fd_);
    }
    fd_ = other.release();
    return *this;
}

int SocketFd::get() const {
    return fd_;
}

bool SocketFd::valid() const {
    return fd_ >= 0;
}

int SocketFd::release() {
    int fd = fd_;
    fd_ = -1;
    return fd;
}

}  // namespace slipstream::net
