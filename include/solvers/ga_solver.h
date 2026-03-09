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
    
    GASolver(const HFT::Graph& graph, 
             const HFT::ExpectedRequests& requests, 
             const HFT::GAConfig& config,
             double max_latency);

    double solve() override;

protected:
    virtual double get_random_double(double min, double max);

    std::vector<std::size_t> stochastic_universal_sampling(const std::vector<double>& population_fitness);
    void crossover(Chromosome& parent1, Chromosome& parent2, int start_idx, int end_idx);
    void mutate(Chromosome& offspring);

private:
    void build_initial_population();
    void reproduce();
    void warm_cache();

    std::mt19937& get_gen();

    std::vector<Chromosome> m_cur_pop_buffer;
    std::vector<Chromosome> m_next_pop_buffer;
    
    SelectionEvaluator m_selection_evaluator;
    double m_best_profit{};

    HFT::GAConfig m_config{};
};

#endif