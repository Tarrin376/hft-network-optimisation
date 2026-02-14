#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <cstdint>

#include "utils/selection_evaluator.h"

#include "types/expected_requests.h"
#include "types/graph.h"

#include "solvers/solver.h"

class BruteForceSolver : public Solver {
public:
    BruteForceSolver(int max_order_profit, double max_latency);

    double solve(const HFTTypes::Graph& graph, const HFTTypes::ExpectedRequests& requests) override;

private:
    double backtrack(const HFTTypes::Graph& graph, 
                     const HFTTypes::ExpectedRequests& requests, 
                     std::vector<uint64_t>& selected_edges, 
                     std::size_t index);
    
    const SelectionEvaluator m_selection_evaluator;
};

#endif