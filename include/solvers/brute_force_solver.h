#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <cstdint>
#include <limits>

#include "utils/selection_evaluator.h"
#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/solver.h"

class BruteForceSolver : public Solver {
public:
    BruteForceSolver(const HFT::Graph& graph, 
                     const HFT::ExpectedRequests& requests, 
                     double max_latency, 
                     bool record_selected_edges);

    double solve() override;

private:
    void backtrack(std::vector<std::uint64_t>& used_edges, std::size_t index);
    void store_selected_edges();
    
    SelectionEvaluator m_selection_evaluator;

    std::vector<std::uint64_t> m_best_edges{};
    double m_best_profit{ std::numeric_limits<double>::lowest() };
};

#endif