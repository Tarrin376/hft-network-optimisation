#include <vector>
#include <cstdint>

#include "solvers/brute_force_solver.h"
#include "solvers/solver.h"

#include "types/expected_requests.h"
#include "types/graph.h"

BruteForceSolver::BruteForceSolver(int max_order_profit, double max_latency) 
    : Solver{ max_order_profit, max_latency }
    , m_selection_evaluator{ max_order_profit, max_latency } {}

double BruteForceSolver::solve(const HFT::Graph& graph, const HFT::ExpectedRequests& requests) {
    std::vector<uint64_t> selected_edges(graph.get_num_edges(), 0);
    auto total_profit{ backtrack(graph, requests, selected_edges, 0) };
    return total_profit;
}

double BruteForceSolver::backtrack(const HFT::Graph& graph,
                                   const HFT::ExpectedRequests& requests, 
                                   std::vector<uint64_t>& selected_edges, 
                                   std::size_t index) {
    if (index == graph.get_num_edges()) {
        return m_selection_evaluator.evaluate(graph, requests, selected_edges);
    }

    selected_edges[index / 64] |= (1ULL << (index % 64));
    auto select_edge_profit{ backtrack(graph, requests, selected_edges, index + 1) };

    selected_edges[index / 64] ^= (1ULL << (index % 64));
    auto ignore_edge_profit{ backtrack(graph, requests, selected_edges, index + 1) };

    return std::max(select_edge_profit, ignore_edge_profit);
}