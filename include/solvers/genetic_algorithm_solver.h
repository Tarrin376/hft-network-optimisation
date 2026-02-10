#ifndef GENETIC_ALGORITHM_SOLVER_H
#define GENETIC_ALGORITHM_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>

#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/ga_config.h"

#include "solvers/solver.h"

class GeneticAlgorithmSolver : public Solver {
public:
    using Chromosome = std::vector<std::uint64_t>;
    
    GeneticAlgorithmSolver(int max_order_profit, double max_latency, const GAConfig& ga);

    double solve(const Graph& graph, const ExpectedRequests& requests) override;

private:
    std::vector<Chromosome> build_initial_population(std::size_t num_edges);
    
    std::vector<Chromosome> reproduce(const Graph& graph, 
                                      const ExpectedRequests& requests, 
                                      std::vector<Chromosome> population);

    std::vector<std::size_t> stochastic_universal_sampling(const std::vector<double>& population_fitness);
    
    void crossover(Chromosome& parent1, Chromosome& parent2, std::size_t num_edges);
    void mutate(Chromosome& offspring);

    double m_max_profit{};
    std::mt19937 m_gen{};
    GAConfig m_ga{};
};

#endif