#pragma once
#include <cstdint>
#include <memory>
#include "slipstream/codec/decoder.hpp"
#include "slipstream/transport/i_feed_transport.hpp"
#include "slipstream/cli/options.hpp"
#include "slipstream/marketdata/l1_book.hpp"

namespace slipstream::server {

// Accepts and serves the unidirectional market-data client connection (quotes only).
class MdServer {
public:
    // book and quote_count are owned by the caller so the OE side can read the same state.
    MdServer(const slipstream::cli::Options& options, slipstream::marketdata::L1Book& book,
             std::uint64_t& quote_count);

    void run();  // Accepts one MD client connection and processes quotes until it disconnects.

    void accept();
    bool on_readable();
    int fd() const;
    void close();

private:
    const slipstream::cli::Options& options_;  // Symbol filter and MD host/port.
    slipstream::marketdata::L1Book& book_;     // Updated from quotes for options_.symbol.
    std::uint64_t& quote_count_;               // Matching quotes seen

    std::unique_ptr<slipstream::transport::IFeedTransport> conn_;
    slipstream::codec::StreamDecoder decoder_;
};

}  // namespace slipstream::server
