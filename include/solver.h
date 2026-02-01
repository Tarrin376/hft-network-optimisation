#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

#include "order_opportunity.h"
#include "graph.h"

class Solver {
public:
    virtual ~Solver() = default;

    virtual double solve(
        const Graph& graph, 
        const std::vector<OrderOpportunity>& order_opportunities
    ) = 0;
};

#endif