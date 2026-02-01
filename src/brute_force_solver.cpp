#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "brute_force_solver.h"
#include "order_opportunity.h"
#include "graph.h"
#include "solver.h"

double BruteForceSolver::solve(
    const Graph& graph, 
    const std::vector<OrderOpportunity>& opportunities
) {
    auto& edges{ graph.get_edge_ids() };
    std::unordered_set<int> selected_edges{};

    double total_profit { find_max_total_profit(edges, selected_edges, 0, graph, opportunities) };
    return total_profit;
}

double BruteForceSolver::find_max_total_profit(
    std::vector<int> edge_ids, std::unordered_set<int>& selected_edges, int index, 
    const Graph& graph, const std::vector<OrderOpportunity>& opportunities) {
    if (index == edge_ids.size()) {
        return find_selected_edges_profit(graph, selected_edges, opportunities);
    }

    selected_edges.insert(edge_ids.at(index));
    double select_edge_profit{ find_max_total_profit(edge_ids, selected_edges, index + 1, graph, opportunities) };

    selected_edges.erase(edge_ids.at(index));
    double ignore_edge_profit{ find_max_total_profit(edge_ids, selected_edges, index + 1, graph, opportunities) };

    return std::max(select_edge_profit, ignore_edge_profit);
}

int BruteForceSolver::find_selected_edges_profit(
    const Graph& graph,
    std::unordered_set<int>& selected_edges, 
    const std::vector<OrderOpportunity>& order_opportunities) {
    // To be implemented
    return 0;
}