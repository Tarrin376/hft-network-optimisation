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
    int get_max_order_flow(const Graph& graph, const Request& request, int remaining_orders);
    int send_orders(const Request& request, std::vector<const Edge*>& parent_edge_buffer, int remaining_orders);

    std::vector<uint8_t> m_selected_edges{}; 
    std::vector<int> m_path_flow{};
};

#endif