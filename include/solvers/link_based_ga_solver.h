#ifndef LINK_BASED_GA_SOLVER_H
#define LINK_BASED_GA_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>
#include <limits>

#include "utils/selection_evaluator.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "interfaces/i_genetic.h"
#include "solvers/solver.h"

class LinkBasedGASolver : public IGenetic, public Solver {
public:
    LinkBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency);

    double solve() override;

protected:
    virtual double get_random_double(double min, double max);

    std::vector<std::size_t> stochastic_universal_sampling(const std::vector<double>& pop_fitness);
    void crossover(Chromosome& parent1, Chromosome& parent2, int start_idx, int end_idx);

    void mutate(Chromosome& offspring) override;

private:
    void build_initial_population() override;
    std::vector<double> get_population_fitness() override;
    double get_chromosome_fitness(const Chromosome& chromosome) override;
    void reproduce() override;

    void warm_cache();
    std::mt19937& get_gen();

    std::uniform_int_distribution<int> m_crossover_dist;
    std::vector<Chromosome> m_cur_pop_buffer;
    std::vector<Chromosome> m_next_pop_buffer;
    
    double m_best_profit{ std::numeric_limits<double>::lowest() };
    HFT::GAConfig m_config{};
};

#endif