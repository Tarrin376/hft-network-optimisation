#ifndef GA_CONFIG_H
#define GA_CONFIG_H

struct GAConfig {
    int population_size = 20;
    int generations = 90;
    double mutation_rate = 0.05;
    double crossover_rate = 0.8;
    double initial_bit_flip_rate = 0.01;
    unsigned int seed = 42;
};

#endif