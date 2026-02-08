#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <cstdint>

namespace BitUtils {
    inline constexpr std::uint64_t mask_left(std::uint64_t value, int position) {
        if (position >= 64) return value;
        if (position <= 0) return 0;
        return value & ((1ULL << position) - 1);
    }

    inline constexpr std::uint64_t mask_right(std::uint64_t value, int position) {
        if (position <= 0) return value;
        if (position >= 64) return 0;
        return value & ~((1ULL << position) - 1);
    }
}

#endif