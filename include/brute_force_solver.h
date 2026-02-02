#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <unordered_set>

#include "order_opportunity.h"
#include "graph.h"
#include "solver.h"

class BruteForceSolver : public Solver {
public:
    BruteForceSolver(int max_order_profit, int max_latency);
    
    struct LatencyPair {
        int latency{};
        const Edge* edge_ptr{};
    };

    int solve(const Graph& graph, const std::vector<OrderOpportunity>& opportunities) override;

private:
    int find_max_profit(const Graph& graph,
                        const std::vector<int> edge_ids, 
                        const std::vector<OrderOpportunity>& opportunities,
                        std::unordered_set<int>& selected_edges, 
                        int index);

    int calculate_total_profit(const Graph& graph,
                               const std::unordered_set<int>& selected_edges, 
                               const std::vector<OrderOpportunity>& opportunities);

    bool find_optimal_path(const Graph& graph, 
                           const OrderOpportunity& opportunity, 
                           const std::unordered_set<int>& selected_edges,
                           std::unordered_map<int, int>& path_flow);
        
    void update_flow_path(const std::unordered_map<int, BruteForceSolver::LatencyPair> latency_costs,
                          std::unordered_map<int, int>& path_flow, 
                          int dest);
};

#endif