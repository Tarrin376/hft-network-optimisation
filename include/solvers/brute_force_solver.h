#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <cstdint>
#include <limits>

#include "utils/selection_evaluator.h"
#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/solver.h"

/**
 * An exhaustive search solver that evaluates all possible edge combinations.
 * 
 * This class uses a backtracking approach to explore the entire solution space
 * of edge selections. Due to its exponential time complexity, it is used 
 * primarily for validating small-scale network topologies.
 */
class BruteForceSolver : public Solver {
public:
    /**
     * @param graph The network topology.
     * @param requests The set of order opportunity requests to be routed.
     * @param max_latency The maximum acceptable latency considered acceptable by the firm.
     * @param record_selected_edges Whether to log final edge selections.
     */
    BruteForceSolver(const HFT::Graph& graph, 
                     const HFT::ExpectedRequests& requests, 
                     double max_latency, 
                     bool record_selected_edges);

    double solve() override;

private:
    void backtrack(std::vector<std::uint64_t>& used_edges, std::size_t index);
    void store_selected_edges();
    
    // Helper utility to evaluate the profit/feasibility of a given network configuration.
    SelectionEvaluator m_selection_evaluator;

    std::vector<std::uint64_t> m_best_edges{};
    double m_best_profit{ std::numeric_limits<double>::lowest() };
};

#endif