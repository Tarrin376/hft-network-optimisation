#include "solvers/brute_force_solver.h"

#include <vector>
#include <cstdint>

#include "types/solver.h"
#include "types/expected_requests.h"
#include "types/graph.h"

BruteForceSolver::BruteForceSolver(const HFT::Graph& graph, 
                                   const HFT::ExpectedRequests& requests, 
                                   double max_latency, 
                                   bool record_selected_edges) 
    : Solver{ graph, requests, max_latency, record_selected_edges }
    , m_selection_evaluator{ max_latency, graph, requests } {}

double BruteForceSolver::solve() {
    std::vector<uint64_t> used_edges(m_graph.get_num_edges(), 0);
    backtrack(used_edges, 0);

    if (m_record_selected_edges) {
        store_selected_edges();
    }

    return m_best_profit;
}

void BruteForceSolver::backtrack(std::vector<uint64_t>& used_edges, std::size_t index) {
    if (index == m_graph.get_num_edges()) {
        double profit = m_selection_evaluator.evaluate(used_edges);
        if (profit > m_best_profit) {
            m_best_profit = profit;
            if (m_record_selected_edges) {
                m_best_edges = used_edges;
            }
        }

        return;
    }

    used_edges[index / 64] |= (1ULL << (index % 64));
    backtrack(used_edges, index + 1);

    used_edges[index / 64] ^= (1ULL << (index % 64));
    backtrack(used_edges, index + 1);
}

void BruteForceSolver::store_selected_edges() {
    for (std::size_t i = 0; i < m_graph.get_num_edges(); ++i) {
        bool is_selected = (m_best_edges[i / 64] & (1ULL << (i % 64))) > 0;
        if (is_selected) {
            m_selected_edges.push_back(i);
        }
    }
}