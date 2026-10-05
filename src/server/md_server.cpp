#include "md_server.hpp"

#include <iostream>
#include <string_view>
#include <variant>

#include "slipstream/codec/decoder.hpp"
#include "slipstream/transport/tcp_feed_transport.hpp"

namespace slipstream::server {

MdServer::MdServer(const slipstream::cli::Options& options, slipstream::marketdata::L1Book& book,
                   std::uint64_t& quote_count)
    : options_{options}, book_{book}, quote_count_{quote_count} {
}

void MdServer::run() {
    auto conn = transport::accept_tcp_feed_transport(options_.md_host, options_.md_port);
    codec::StreamDecoder decoder;
    std::uint8_t buffer[4096];
    std::size_t bytes{};

    while ((bytes = conn->receive(buffer, sizeof(buffer))) > 0) {
        decoder.feed(buffer, bytes);
        while (auto msg = decoder.try_decode_next()) {
            const auto* quote = std::get_if<codec::Quote>(&*msg);
            if (quote == nullptr) {
                continue;
            }
            std::string_view sv{quote->symbol, 20};
            while (!sv.empty() && sv.back() == '\0') {
                sv.remove_suffix(1);
            }
            if (sv == options_.symbol) {
                book_.on_quote(*quote);
                ++quote_count_;
            } else {
                std::cout << "dropped quote for " << symbol << '\n';
            }
        }
    }
}

}  // namespace slipstream::server
