#ifndef STOCHASTIC_UNIVERSAL_SAMPLING_H
#define STOCHASTIC_UNIVERSAL_SAMPLING_H

#include <vector>
#include <random>
#include <cstdint>

#include "utils/random_utils.h"

struct StochasticUniversalSampling {
    void run(const std::vector<double>& fitness, std::vector<std::size_t>& next_gen_parents, std::mt19937& gen) {
        std::size_t population_size{ fitness.size() };
        double total{ std::accumulate(fitness.begin(), fitness.end(), 0.0) };

        const double step{ total / static_cast<double>(population_size) };
        double cumulative{ fitness[0] };

        double pointer{ RandomUtils::get_random_double(0.0, step, gen) };
        std::size_t idx{ 0 };

        for (std::size_t i = 0; i < population_size; ++i) {
            while (pointer > cumulative && idx < population_size - 1) {
                ++idx;
                cumulative += fitness[idx];
            }

            next_gen_parents[i] = idx;
            pointer += step;
        }
    }
};

#endif