#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include <random>

/**
 * This namespace provides helper functions that wrap standard library 
 * distributions to simplify common randomisation tasks while maintaining 
 * efficiency and thread safety.
 */
namespace RandomUtils {
    /**
     * Generates a random floating-point number within a specified range.
     * It uses a thread_local distribution to ensure thread safety 
     * and avoid the overhead of re-initialising the distribution 
     * object on every call.
     * 
     * @param min The inclusive lower bound of the range.
     * @param max The inclusive upper bound of the range.
     * @param gen A reference to the Mersenne Twister engine used for generation.
     * @return A random value that is between 'min' and 'max' (inclusively).
     */
    inline double get_random_double(double min, double max, std::mt19937& gen) {
        static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
        return min + (max - min) * dist(gen);
    }
}

#endif