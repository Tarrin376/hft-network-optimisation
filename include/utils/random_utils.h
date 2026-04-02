#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include <random>

namespace RandomUtils {
    inline double get_random_double(double min, double max, std::mt19937& gen) {
        static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
        return min + (max - min) * dist(gen);
    }
}

#endif