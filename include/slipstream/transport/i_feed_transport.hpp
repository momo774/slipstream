#pragma once
#include <cstddef>
#include <cstdint>

namespace slipstream::transport {

// Abstract byte-stream transport used by the server for both MD and OE connections
class IFeedTransport {
public:
    virtual ~IFeedTransport() = default;

    virtual std::size_t receive(std::uint8_t* buf, std::size_t len) = 0;  // Blocking read; 0 on close.
    virtual void send(const std::uint8_t* buf, std::size_t len) = 0;      // Blocking full write.
    virtual void close() = 0;                                            // Closes the connection.
};

}  // namespace slipstream::transport
