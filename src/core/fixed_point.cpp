#include "slipstream/core/fixed_point.hpp"

namespace slipstream::core {

Price to_fixed_point(double realDoublePrice) {
    return realDoublePrice * kPriceScale;
}

double to_display_price(Price scaledPrice) {
    return static_cast<double>(scaledPrice) / static_cast<double>(kPriceScale);
}

}  // namespace slipstream::core
