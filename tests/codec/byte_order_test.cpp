#include <gtest/gtest.h>
#include "slipstream/codec/byte_order.hpp"

#include <cstdint>
#include <vector>

using namespace slipstream::codec;

TEST(ByteOrderTest, WriteU16IsLittleEndian) {
    std::vector<std::uint8_t> buf;
    write_u16_le(buf, 0x1234);
    EXPECT_EQ(buf, (std::vector<std::uint8_t>{0x34, 0x12}));
}

TEST(ByteOrderTest, WriteU32IsLittleEndian) {
    std::vector<std::uint8_t> buf;
    write_u32_le(buf, 0x12345678);
    EXPECT_EQ(buf, (std::vector<std::uint8_t>{0x78, 0x56, 0x34, 0x12}));
}

TEST(ByteOrderTest, WriteU64IsLittleEndian) {
    std::vector<std::uint8_t> buf;
    write_u64_le(buf, 0x0102030405060708ULL);
    EXPECT_EQ(buf, (std::vector<std::uint8_t>{0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01}));
}

TEST(ByteOrderTest, ReadReconstructsValues) {
    std::vector<std::uint8_t> buf{0x34, 0x12, 0x78, 0x56, 0x34, 0x12};
    EXPECT_EQ(read_u16_le(buf, 0), 0x1234);
    EXPECT_EQ(read_u32_le(buf, 2), 0x12345678u);
}

TEST(ByteOrderTest, ReadU64Reconstructs) {
    std::vector<std::uint8_t> buf{0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    EXPECT_EQ(read_u64_le(buf, 0), 0x0102030405060708ULL);
}
