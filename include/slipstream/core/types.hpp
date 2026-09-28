#pragma once
#include <cstdint>

namespace slipstream::core {

enum class Side : std::uint8_t { Buy = 0, Sell = 1 };  // Internal side representation used by strategy code.

using Price = std::int64_t;  // Fixed-point price; real value = Price / kPriceScale.

}  // namespace slipstream::core
