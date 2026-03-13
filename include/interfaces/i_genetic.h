#ifndef I_GENETIC_H
#define I_GENETIC_H

#include <vector>
#include <cstdint>

class IGenetic {
public:
    using Chromosome = std::vector<std::uint64_t>;

    virtual ~IGenetic() = default;

    virtual void build_initial_population() = 0;
    virtual std::vector<double> get_population_fitness() = 0;
    virtual double get_chromosome_fitness(const Chromosome& chromosome) = 0;
    virtual void reproduce() = 0;
    virtual void mutate(Chromosome& offspring) = 0;
};

#endif