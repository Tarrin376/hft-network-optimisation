#ifndef SELECTION_EVALUATOR_H
#define SELECTION_EVALUATOR_H

#include <vector>
#include <cstdint>

#include "types/expected_requests.h"
#include "types/graph.h"

class SelectionEvaluator final {
public:
    SelectionEvaluator(double max_latency, const HFT::Graph& graph, const HFT::ExpectedRequests& requests);

    double evaluate(const std::vector<std::uint64_t>& selected_edges);

    const std::vector<std::size_t>& get_used_edges() const;

private:
    int update_path_flow(const HFT::Request& request, 
                         const std::vector<std::uint64_t>& selected_edges,
                         int remaining_orders);

    int process_orders(const HFT::Request& request, int remaining_orders);

    bool edge_is_selected(std::size_t edge_index, const std::vector<std::uint64_t>& selected_edges) const;

    std::vector<double> m_min_latency_buffer;
    std::vector<const HFT::Edge*> m_parent_edge_buffer;
    std::vector<int> m_path_flow;
    std::vector<std::uint64_t> m_used_edges;

    const double m_max_latency{};
    const HFT::Graph& m_graph;
    const HFT::ExpectedRequests& m_requests;
};

#endif