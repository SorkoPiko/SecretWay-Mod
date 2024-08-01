#include <algorithm>
#include "Utils.hpp"

int Utils::capNumber(const int value, const int min, const int max) {
    return std::max(min, std::min(value, max));
}