#ifndef SELECTION_EVALUATOR_H
#define SELECTION_EVALUATOR_H

#include <vector>
#include <cstdint>

#include "types/expected_requests.h"
#include "types/graph.h"

class SelectionEvaluator {
public:
    SelectionEvaluator(int max_order_profit, double max_latency);

    double evaluate(const Graph& graph, 
                    const ExpectedRequests& requests, 
                    const std::vector<uint8_t>& selected_edges) const;

private:
    double find_total_profit(const Graph& graph, 
                             const ExpectedRequests& requests, 
                             const std::vector<uint8_t>& selected_edges) const;

    int get_processed_orders(const Graph& graph, 
                             const Request& request, 
                             const std::vector<uint8_t>& selected_edges,
                             std::vector<int>& path_flow,
                             int remaining_orders) const;

    int process_orders(const Request& request, 
                       std::vector<const Edge*>& parent_edge_buffer,
                       std::vector<int>& path_flow,
                       int remaining_orders) const;

    int m_max_order_profit{};
    double m_max_latency{};
};

#endif