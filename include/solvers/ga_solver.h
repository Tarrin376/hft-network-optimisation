#ifndef GA_SOLVER_H
#define GA_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>

#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"

#include "solvers/ga_solver.h"
#include "solvers/solver.h"

class GASolver : public Solver {
public:
    using Chromosome = std::vector<std::uint64_t>;
    
    GASolver(int max_order_profit, double max_latency, const GAConfig& ga);

    double solve(const Graph& graph, const ExpectedRequests& requests);

protected:
    virtual double get_random_double(double min, double max);

    std::vector<std::size_t> stochastic_universal_sampling(const std::vector<double>& population_fitness);

    void crossover(Chromosome& parent1, Chromosome& parent2, int start_idx, int end_idx);
    void mutate(Chromosome& offspring);

    std::mt19937 m_gen{};
    GAConfig m_ga{};

private:
    std::vector<Chromosome> build_initial_population(std::size_t num_edges);
    std::vector<Chromosome> reproduce(const Graph& graph, const ExpectedRequests& requests, std::vector<Chromosome> population);

    double m_max_profit{};
};

#endif