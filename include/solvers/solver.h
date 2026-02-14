#ifndef SOLVER_H
#define SOLVER_H

#include "types/expected_requests.h"
#include "types/graph.h"

#include "utils/selection_evaluator.h"

class Solver {
public:
    virtual ~Solver() = default;

    virtual double solve(
        const HFTTypes::Graph& graph, 
        const HFTTypes::ExpectedRequests& requests
    ) = 0;

protected:
    Solver(int max_order_profit, double max_latency) 
    : m_selection_evaluator{ max_order_profit, max_latency } {}

    const SelectionEvaluator m_selection_evaluator;
};

#endif