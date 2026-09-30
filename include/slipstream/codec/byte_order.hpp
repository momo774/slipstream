#pragma once
#include <cstdint>
#include <vector>
#include <span>

namespace slipstream::codec {

void write_u16_le(std::vector<std::uint8_t>& buffer, std::uint16_t value);
void write_u32_le(std::vector<std::uint8_t>& buffer, std::uint32_t value);
void write_u64_le(std::vector<std::uint8_t>& buffer, std::uint64_t value);
std::uint16_t read_u16_le(std::span<const std::uint8_t> data, std::size_t offset);
std::uint32_t read_u32_le(std::span<const std::uint8_t> data, std::size_t offset);
std::uint64_t read_u64_le(std::span<const std::uint8_t> data, std::size_t offset);
};   // namespace slipstream::codec





