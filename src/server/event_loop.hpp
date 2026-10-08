#pragma once
#include "md_server.hpp"
#include "oe_server.hpp"

namespace slipstream::server {

// Single-threaded poll() loop over the MD socket, the OE socket and stdin. Returns when either
// client disconnects or the operator types CLOSE.
void run_event_loop(MdServer& md, OeServer& oe);

}  // namespace slipstream::server
