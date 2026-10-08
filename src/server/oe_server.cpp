#include "oe_server.hpp"

#include <chrono>
#include <cstring>
#include <iostream>
#include <string_view>
#include <variant>

#include "slipstream/codec/encoder.hpp"
#include "slipstream/core/fixed_point.hpp"
#include "slipstream/transport/tcp_feed_transport.hpp"

namespace slipstream::server {

OeServer::OeServer(const slipstream::cli::Options& options, const slipstream::marketdata::L1Book& book,
                   const std::uint64_t& quote_count, slipstream::marketdata::RollingVwap& vwap,
                   slipstream::strategy::DecisionEngine& engine)
    : options_{options}, book_{book}, quote_count_{quote_count}, vwap_{vwap}, engine_{engine} {
}

void OeServer::accept() {
    auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    conn_ = transport::accept_tcp_feed_transport(options_.oe_host, options_.oe_port);
    session_.emplace(*conn_);
    heartbeat_.emplace(*conn_);
    heartbeat_->on_heartbeat_received(now);
}

bool OeServer::on_readable() {
    std::uint8_t buffer[4096];
    std::size_t bytes{};
    bytes = conn_->receive(buffer, sizeof(buffer));
    if (bytes <= 0) {
        return false;
    }
    decoder_.feed(buffer, bytes);
    auto now = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    heartbeat_->on_heartbeat_received(now);

    while (auto msg = decoder_.try_decode_next()) {
        const auto* trade = std::get_if<codec::Trade>(&*msg);
        if (trade == nullptr) {
            continue;
        }
        std::string_view sv{trade->symbol, 12};
        while (!sv.empty() && sv.back() == '\0') {
            sv.remove_suffix(1);
        }
        if (sv == options_.symbol) {
            handle_trade(*trade);
        } else {
            std::cout << "dropped trade for " << sv << '\n';
        }
    }
    return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
void OeServer::handle_trade(const slipstream::codec::Trade& trade) {
    const std::uint32_t qty = trade.qty;
    const std::int64_t px = trade.px;
    vwap_.on_trade(trade);
    engine_.on_market_trade(qty);
    market_qty_ += qty;
    session_px_qty_ += static_cast<__int128>(px) * qty;

    if (session_->current_state() == session::SessionState::Halted) return;

    const auto decision = engine_.evaluate(trade, book_, quote_count_);
    if (decision == strategy::Decision::NoAction) return;   // still warming up

    codec::NewOrder order{};
    order.client_order_id = next_order_id_++;
    std::memcpy(order.symbol, trade.symbol, sizeof(order.symbol));
    order.status = decision == strategy::Decision::Reject ? 'R' : 'A';
    order.ts_ns = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    order.trade_id = trade.id;
    order.side = decision == strategy::Decision::Sell ? 'S' : 'B';
    order.qty = qty;
    order.limit_px = px;

    if (order.status == 'A') {
        engine_.on_own_fill(qty);
        executed_qty_ += qty;
        executed_px_qty_ += static_cast<__int128>(px) * qty;
    }

    const auto bytes = codec::encode_new_order(order);
    conn_->send(bytes.data(), bytes.size());
}
#pragma GCC diagnostic pop

void OeServer::on_stdin() {
    session_->poll_stdin();
}

void OeServer::on_tick(std::uint64_t now_ns) {
    if (!heartbeat_ || !heartbeat_->is_stale(now_ns)) {
        return;
    }
    std::cerr << "warning: no OE client traffic for 5s, sending heartbeat\n";
    heartbeat_->send_heartbeat(now_ns);
    heartbeat_->on_heartbeat_received(now_ns);
}

bool OeServer::closed() const {
    return session_ && session_->current_state() == slipstream::session::SessionState::Closed;
}

int OeServer::fd() const {
    return conn_ ? conn_->fd() : -1;
}

void OeServer::close() {
    if (conn_) {
        conn_->close();
    }
}

slipstream::report::ExecutionReport OeServer::build_report() const {
    const auto scale = static_cast<double>(slipstream::core::kPriceScale);
    const auto market = static_cast<double>(market_qty_);
    const auto executed = static_cast<double>(executed_qty_);

    slipstream::report::ExecutionReport report{};
    report.symbol = options_.symbol;
    report.market_qty = market_qty_;
    report.executed_qty = executed_qty_;
    report.avg_fill_price = executed_qty_ == 0 ? 0.0 : static_cast<double>(executed_px_qty_) / executed / scale;
    report.session_vwap = market_qty_ == 0 ? 0.0 : static_cast<double>(session_px_qty_) / market / scale;
    report.slippage_bps = (executed_qty_ == 0 || report.session_vwap == 0.0)
                              ? 0.0
                              : (report.avg_fill_price - report.session_vwap) / report.session_vwap * 10'000;
    report.participation_rate = market_qty_ == 0 ? 0.0 : executed / market;
    report.participation_cap = options_.participation_cap;
    return report;
}

}  // namespace slipstream::server
