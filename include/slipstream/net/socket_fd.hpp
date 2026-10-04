#pragma once

namespace slipstream::net {

// RAII wrapper that owns a raw POSIX socket file descriptor and closes it on destruction.
class SocketFd {
public:
    SocketFd() = default;
    explicit SocketFd(int fd);
    ~SocketFd();

    SocketFd(const SocketFd&) = delete;
    SocketFd& operator=(const SocketFd&) = delete;
    SocketFd(SocketFd&& other) noexcept;
    SocketFd& operator=(SocketFd&& other) noexcept;

    int get() const;     // Raw fd, or -1 if empty.
    bool valid() const;  // True if this wrapper owns an open fd.
    int release();       // Releases ownership without closing.

private:
    int fd_ = -1;  // Owned file descriptor, or -1 if empty.
};

}  // namespace slipstream::net
