#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <cstdint>

#include "utils/selection_evaluator.h"

#include "types/expected_requests.h"
#include "types/graph.h"

#include "solvers/solver.h"

class BruteForceSolver final : public Solver {
public:
    BruteForceSolver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency);

    double solve() override;

private:
    double backtrack(std::vector<uint64_t>& selected_edges, std::size_t index);
    
    SelectionEvaluator m_selection_evaluator;
};

#endif