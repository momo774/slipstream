#include "event_loop.hpp"

#include <poll.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <system_error>

namespace slipstream::server {

namespace {
constexpr int kPollTimeoutMs = 100;  // Arbitrary

std::uint64_t steady_now_ns() {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
}


void run_event_loop(MdServer& md, OeServer& oe) {
    pollfd fds[3] = {
        {md.fd(), POLLIN, 0},
        {oe.fd(), POLLIN, 0},
        {STDIN_FILENO, POLLIN, 0},
    };

    while (true) {
        const int ready = ::poll(fds, 3, kPollTimeoutMs);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            throw std::system_error(errno, std::generic_category(), "poll");
        }

        constexpr short kReadable = POLLIN | POLLHUP | POLLERR;
        if ((fds[0].revents & kReadable) && !md.on_readable()) {
            break;  // MD client disconnected.
        }
        if ((fds[1].revents & kReadable) && !oe.on_readable()) {
            break;  // OE client disconnected.
        }
        if (fds[2].revents & POLLIN) {
            oe.on_stdin();
            if (oe.closed()) {
                break;  // Operator typed CLOSE.
            }
            if (std::cin.eof()) {
                fds[2].fd = -1;  // stdin closed: poll() ignores negative fds, so stop watching it.
            }
        }

        oe.on_tick(steady_now_ns());
    }

    md.close();
    oe.close();
}

}  // namespace slipstream::server
