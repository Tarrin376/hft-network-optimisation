#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"

#include "solvers/genetic_algorithm_solver.h"
#include "solvers/solver.h"

GeneticAlgorithmSolver::GeneticAlgorithmSolver(int max_order_profit, double max_latency, const GAConfig& ga)
    : Solver{ max_order_profit, max_latency }
    , m_ga{ ga } {}

double GeneticAlgorithmSolver::solve(const Graph& graph, const ExpectedRequests& requests) const {
    return 0;
}