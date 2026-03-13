#include "solvers/brute_force_solver.h"

#include <vector>
#include <cstdint>

#include "types/solver.h"
#include "types/expected_requests.h"
#include "types/graph.h"

BruteForceSolver::BruteForceSolver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency) 
    : Solver{ graph, requests, max_latency }
    , m_selection_evaluator{ max_latency, graph, requests } {}

double BruteForceSolver::solve() {
    std::vector<uint64_t> selected_edges(m_graph.get_num_edges(), 0);
    auto total_profit{ backtrack(selected_edges, 0) };
    return total_profit;
}

double BruteForceSolver::backtrack(std::vector<uint64_t>& selected_edges, std::size_t index) {
    if (index == m_graph.get_num_edges()) {
        return m_selection_evaluator.evaluate(selected_edges);
    }

    selected_edges[index / 64] |= (1ULL << (index % 64));
    auto select_edge_profit{ backtrack(selected_edges, index + 1) };

    selected_edges[index / 64] ^= (1ULL << (index % 64));
    auto ignore_edge_profit{ backtrack(selected_edges, index + 1) };

    return std::max(select_edge_profit, ignore_edge_profit);
}