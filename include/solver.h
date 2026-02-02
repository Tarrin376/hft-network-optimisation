#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

#include "order_opportunity.h"
#include "graph.h"

class Solver {
public:
    virtual ~Solver() = default;

    virtual int solve(
        const Graph& graph, 
        const std::vector<OrderOpportunity>& order_opportunities
    ) = 0;

protected:
    Solver(int max_order_profit, int max_latency) 
    : m_max_order_profit{ max_order_profit }
    , m_max_latency{ max_latency } {}

    int m_max_order_profit{};
    int m_max_latency{};
};

#endif