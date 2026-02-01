#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <unordered_set>

#include "order_opportunity.h"
#include "graph.h"
#include "solver.h"

class BruteForceSolver : public Solver {
public:
    double solve(
        const Graph& graph, 
        const std::vector<OrderOpportunity>& opportunities
    ) override;

private:
    double find_max_total_profit(
        std::vector<int> edge_ids, std::unordered_set<int>& selected_edges, int index, 
        const Graph& graph, const std::vector<OrderOpportunity>& opportunities);

    int find_selected_edges_profit(
        const Graph& graph,
        std::unordered_set<int>& selected_edges, 
        const std::vector<OrderOpportunity>& opportunities);
};

#endif