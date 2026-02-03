#ifndef BRUTE_FORCE_SOLVER_H
#define BRUTE_FORCE_SOLVER_H

#include <vector>
#include <cstdint>

#include "types/expected_requests.h"
#include "types/graph.h"
#include "solvers/solver.h"

class BruteForceSolver : public Solver {
public:
    BruteForceSolver(int max_order_profit, double max_latency);

    double solve(const Graph& graph, const ExpectedRequests& requests) override;

private:
    double find_max_profit(const Graph& graph, const ExpectedRequests& requests, std::size_t index);
    double calculate_total_profit(const Graph& graph, const ExpectedRequests& requests);
    bool find_optimal_path(const Graph& graph, const Request& request);
    void update_flow_path(int target_exchange, const std::vector<const Edge*>& parent_edge_buffer);

    std::vector<uint8_t> m_selected_edges{}; 
    std::vector<int> m_path_flow{};
};

#endif