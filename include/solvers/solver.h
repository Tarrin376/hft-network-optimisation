#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

#include "types/expected_requests.h"
#include "types/graph.h"

class Solver {
public:
    virtual ~Solver() = default;

    virtual double solve(
        const Graph& graph, 
        const ExpectedRequests& requests
    ) = 0;

protected:
    Solver(int max_order_profit, double max_latency) 
    : m_max_order_profit{ max_order_profit }
    , m_max_latency{ max_latency } {}

    int m_max_order_profit{};
    double m_max_latency{};
};

#endif