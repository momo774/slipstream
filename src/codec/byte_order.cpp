#include "slipstream/codec/byte_order.hpp"
#include <cstdint>
#include <span>
#include <vector>

namespace slipstream::codec {

void write_u16_le(std::vector<std::uint8_t>& buffer, std::uint16_t value) {
    for (auto i{0}; i < 2; ++i) {
        buffer.push_back((value >> 8 * i) & 0xFF);
    }
}
void write_u32_le(std::vector<std::uint8_t>& buffer, std::uint32_t value) {
    for (auto i{0}; i < 4; ++i) {
        buffer.push_back((value >> 8 * i) & 0xFF);
    }
}
void write_u64_le(std::vector<std::uint8_t>& buffer, std::uint64_t value) {
    for (auto i{0}; i < 8; ++i) {
        buffer.push_back((value >> 8 * i) & 0xFF);
    }
}
std::uint16_t read_u16_le(std::span<const std::uint8_t> data, std::size_t offset) {
    std::uint16_t value{};
    for (auto i{0}; i < 2; ++i) {
        value |= static_cast<uint16_t>(data[offset + i]) << (8 * i);
    }
    return value;
}
std::uint32_t read_u32_le(std::span<const std::uint8_t> data, std::size_t offset) {
    std::uint32_t value{};
    for (auto i{0}; i < 4; ++i) {
        value |= static_cast<uint32_t>(data[offset + i]) << (8 * i);
    }
    return value;
}
std::uint64_t read_u64_le(std::span<const std::uint8_t> data, std::size_t offset) {
    std::uint64_t value{};
    for (auto i{0}; i < 8; ++i) {
        value |= static_cast<uint64_t>(data[offset + i]) << (8 * i);
    }
    return value;
}
}   // namespace slipstream::codec





