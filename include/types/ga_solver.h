#ifndef I_GENETIC_H
#define I_GENETIC_H

#include <vector>
#include <cstdint>
#include <limits>
#include <random>

#include "types/solver.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/expected_requests.h"

class GASolver : public Solver {
public:
    using Chromosome = std::vector<std::uint64_t>;

    struct FitnessPair {
        double fitness;
        std::vector<std::size_t> selected_edges;
    };

    GASolver(const HFT::Graph& graph, 
             const HFT::ExpectedRequests& requests, 
             const HFT::GAConfig& config,
             double max_latency,
             std::size_t chromosome_size,
             bool record_selected_edges);

    virtual ~GASolver() = default;

    double solve() override;

protected:
    virtual bool build_initial_population() = 0;
    virtual FitnessPair get_chromosome_fitness(const Chromosome& chromosome) = 0;
    virtual void mutate(Chromosome& offspring) = 0;
    virtual void crossover(Chromosome& parent1, Chromosome& parent2) = 0;
    virtual void warm_cache() = 0;

    virtual std::vector<std::size_t> select_next_gen_parents(const std::vector<double>& pop_fitness);
    virtual std::vector<double> get_population_fitness();
    virtual double get_random_double(double min, double max);
    
    std::mt19937& get_gen();
    void reproduce();

    std::vector<Chromosome> m_cur_pop_buffer;
    std::vector<Chromosome> m_next_pop_buffer;
    HFT::GAConfig m_config{};
    
    double m_best_profit{ std::numeric_limits<double>::lowest() };
};

#endif