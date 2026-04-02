#ifndef STOCHASTIC_UNIVERSAL_SAMPLING_H
#define STOCHASTIC_UNIVERSAL_SAMPLING_H

#include <vector>
#include <random>
#include <cstdint>

struct StochasticUniversalSampling {
    void run(const std::vector<double>& fitness, std::vector<std::size_t>& next_gen_parents, std::mt19937& gen);
};

#endif