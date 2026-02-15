#ifndef GA_SOLVER_H
#define GA_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>

#include "utils/selection_evaluator.h"

#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"

#include "solvers/ga_solver.h"
#include "solvers/solver.h"

class GASolver : public Solver {
public:
    using Chromosome = std::vector<std::uint64_t>;
    
    GASolver(int max_order_profit, double max_latency, const HFT::GAConfig& config);

    double solve(const HFT::Graph& graph, const HFT::ExpectedRequests& requests);

protected:
    virtual double get_random_double(double min, double max);

    std::vector<std::size_t> stochastic_universal_sampling(const std::vector<double>& population_fitness);

    void crossover(Chromosome& parent1, Chromosome& parent2, int start_idx, int end_idx);
    void mutate(Chromosome& offspring);

    std::mt19937 m_gen{};
    HFT::GAConfig m_config{};

private:
    std::vector<Chromosome> build_initial_population(std::size_t num_edges);
    
    std::vector<Chromosome> reproduce(const HFT::Graph& graph, 
                                      const HFT::ExpectedRequests& requests, 
                                      std::vector<Chromosome> population);
    
    const SelectionEvaluator m_selection_evaluator;
    double m_best_profit_achieved{};
};

#endif