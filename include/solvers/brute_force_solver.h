#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <cstdint>

#include "types/order_opportunity.h"
#include "types/graph.h"
#include "solvers/solver.h"

class BruteForceSolver : public Solver {
public:
    BruteForceSolver(int max_order_profit, int max_latency);

    int solve(const Graph& graph, const std::vector<OrderOpportunity>& opportunities) override;

private:
    int find_max_profit(const Graph& graph,
                        const std::vector<OrderOpportunity>& opportunities,
                        std::size_t index);

    int calculate_total_profit(const Graph& graph, const std::vector<OrderOpportunity>& opportunities);
    bool find_optimal_path(const Graph& graph, const OrderOpportunity& opportunity);
    void update_flow_path(int target_exchange, const std::vector<const Edge*>& parent_edge_buffer);

    std::vector<uint8_t> m_selected_edges{}; 
    std::vector<int> m_path_flow{};
};

#endif