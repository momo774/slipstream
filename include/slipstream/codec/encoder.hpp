#pragma once
#include <cstdint>
#include <vector>
#include "slipstream/codec/md_messages.hpp"
#include "slipstream/codec/oe_messages.hpp"

namespace slipstream::codec {

std::vector<std::uint8_t> encode_quote(const Quote& quote);                 // Frames + serializes a Quote.
std::vector<std::uint8_t> encode_trade(const Trade& trade);                 // Frames + serializes a Trade.
std::vector<std::uint8_t> encode_heartbeat(const Heartbeat& heartbeat);     // Frames + serializes a Heartbeat.
std::vector<std::uint8_t> encode_session_control(const SessionControl& sc); // Frames + serializes a SessionControl.
std::vector<std::uint8_t> encode_new_order(const NewOrder& order);          // Frames + serializes a NewOrder.
std::vector<std::uint8_t> encode_exec_report(const ExecReport& report);     // Frames + serializes an ExecReport.

}  // namespace slipstream::codec
