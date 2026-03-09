#ifndef SELECTION_EVALUATOR_H
#define SELECTION_EVALUATOR_H

#include <vector>
#include <cstdint>

#include "types/expected_requests.h"
#include "types/graph.h"

class SelectionEvaluator final {
public:
    SelectionEvaluator(double max_latency);

    double evaluate(const HFT::Graph& graph, 
                    const HFT::ExpectedRequests& requests, 
                    const std::vector<uint64_t>& selected_edges) const;

private:
    int get_processed_orders(const HFT::Graph& graph, 
                             const HFT::Request& request, 
                             const std::vector<uint64_t>& selected_edges,
                             std::vector<int>& path_flow,
                             int remaining_orders) const;

    int process_orders(const HFT::Request& request, 
                       std::vector<const HFT::Edge*>& parent_edge_buffer,
                       std::vector<int>& path_flow,
                       int remaining_orders) const;

    bool edge_is_selected(std::size_t edge_index, const std::vector<uint64_t>& selected_edges) const;

    const double m_max_latency{};
};

#endif