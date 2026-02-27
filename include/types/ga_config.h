#ifndef GA_CONFIG_H
#define GA_CONFIG_H

namespace HFT {
    struct GAConfig {
        int population_size = 100;
        int generations = 300;
        double mutation_rate = 0.05;
        double crossover_rate = 0.8;
        double initial_bit_flip_rate = 0.01;
        unsigned long long seed = 34ULL;
    };
}

#endif