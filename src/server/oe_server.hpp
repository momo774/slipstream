#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include "slipstream/cli/options.hpp"
#include "slipstream/codec/decoder.hpp"
#include "slipstream/codec/md_messages.hpp"
#include "slipstream/marketdata/l1_book.hpp"
#include "slipstream/marketdata/rolling_vwap.hpp"
#include "slipstream/report/execution_report.hpp"
#include "slipstream/session/heartbeat_monitor.hpp"
#include "slipstream/session/session_controller.hpp"
#include "slipstream/strategy/decision_engine.hpp"
#include "slipstream/transport/i_feed_transport.hpp"

namespace slipstream::server {

// Accepts and serves the bidirectional order-entry connection (orders out, trades/heartbeats/acks in).
class OeServer {
public:
    // Shared state is owned by the caller: book/quote_count are written by MdServer, vwap/engine here.
    OeServer(const slipstream::cli::Options& options, const slipstream::marketdata::L1Book& book,
             const std::uint64_t& quote_count, slipstream::marketdata::RollingVwap& vwap,
             slipstream::strategy::DecisionEngine& engine);

    void accept();                     // Blocks until the OE client connects; creates session/heartbeat.
    bool on_readable();                // One receive + decode pass; false once the client has disconnected.
    void on_stdin();                   // Operator typed a line (HALT/OPEN/CLOSE).
    void on_tick(std::uint64_t now_ns);  // Periodic poll() timeout: heartbeat staleness check.
    bool closed() const;               // True after CLOSE.
    int fd() const;                    // Connected socket, for the event loop's poll().
    void close();                      // Tears down the OE connection.

    slipstream::report::ExecutionReport build_report() const;  // Snapshot of the run's totals.

private:
    void handle_trade(const slipstream::codec::Trade& trade);

    const slipstream::cli::Options& options_;
    const slipstream::marketdata::L1Book& book_;
    const std::uint64_t& quote_count_;
    slipstream::marketdata::RollingVwap& vwap_;
    slipstream::strategy::DecisionEngine& engine_;

    std::unique_ptr<slipstream::transport::IFeedTransport> conn_;
    slipstream::codec::StreamDecoder decoder_;
    std::optional<slipstream::session::SessionController> session_;   // Needs conn_, so built in accept().
    std::optional<slipstream::session::HeartbeatMonitor> heartbeat_;  // Same.

    std::uint64_t next_order_id_ = 1;  // NewOrder.client_order_id, strictly increasing.

    // Report totals. Session VWAP covers every trade since start, unlike the rolling window.
    std::uint64_t market_qty_ = 0;
    std::uint64_t executed_qty_ = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    __int128 session_px_qty_ = 0;   // Sum px*qty over all trades for options_.symbol.
    __int128 executed_px_qty_ = 0;  // Sum px*qty over accepted orders.
#pragma GCC diagnostic pop
};

}  // namespace slipstream::server
