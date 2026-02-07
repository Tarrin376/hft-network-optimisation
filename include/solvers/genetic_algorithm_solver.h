#ifndef GENETIC_ALGORITHM_SOLVER_H
#define GENETIC_ALGORITHM_SOLVER_H

#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/ga_config.h"

#include "solvers/solver.h"

class GeneticAlgorithmSolver : public Solver {
public:
    GeneticAlgorithmSolver(int max_order_profit, double max_latency, const GAConfig& ga);

    double solve(const Graph& graph, const ExpectedRequests& requests) const override;

private:
    GAConfig m_ga{};
};

#endif