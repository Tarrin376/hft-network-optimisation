#ifndef GA_CONFIG_H
#define GA_CONFIG_H

struct GAConfig {
    int population = 200;
    int generations = 500;
    double mutation_rate = 0.05;
    double crossover_rate = 0.8;
    int tournament_k = 3;
    unsigned seed = 42;
};

#endif