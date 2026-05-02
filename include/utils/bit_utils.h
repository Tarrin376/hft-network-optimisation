#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <cstdint>

/**
 * Low-level bit manipulation utilities for chromosome and bitset operations.
 * 
 * These functions provide efficient masking for 64-bit integers, used 
 * in genetic algorithm crossover points.
 */
namespace BitUtils {
    /**
     * Returns a value containing only the bits to the left of the specified position.
     * "Left" in this context refers to the least significant bits (LSB) up to 
     * the given index.
     * @param value The 64-bit integer to mask.
     * @param position The bit index (0-63) acting as the boundary.
     * @return The masked value.
     */
    inline constexpr std::uint64_t mask_left(std::uint64_t value, int position) {
        if (position >= 64) return value;
        if (position <= 0) return 0;
        return value & ((1ULL << position) - 1);
    }

    /**
     * Returns a value containing only the bits to the right of the specified position.
     * "Right" in this context refers to the most significant bits (MSB) starting 
     * from the given index.
     * @param value The 64-bit integer to mask.
     * @param position The bit index (0-63) acting as the boundary.
     * @return The masked value.
     */
    inline constexpr std::uint64_t mask_right(std::uint64_t value, int position) {
        if (position <= 0) return value;
        if (position >= 64) return 0;
        return value & ~((1ULL << position) - 1);
    }
}

#endif