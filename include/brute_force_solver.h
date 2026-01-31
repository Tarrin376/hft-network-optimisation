#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include "order_opportunity.h"
#include "graph.h"
#include "solver.h"

class BruteForceSolver : public Solver {
public:
    void solve(
        const Graph& graph, 
        const std::vector<OrderOpportunity>& order_opportunities
    ) override;
};

#endif