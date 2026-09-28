#pragma once
#include "slipstream/core/types.hpp"

namespace slipstream::core {

inline constexpr std::int64_t kPriceScale = 10'000;  // Fixed-point scale applied to all wire/internal prices.

Price to_fixed_point(double real_price);      // Converts a decimal price into scaled fixed-point form.
double to_display_price(Price scaled_price);  // Converts scaled fixed-point back to double, for display only.

}  // namespace slipstream::core
