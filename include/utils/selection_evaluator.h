#ifndef SELECTION_EVALUATOR_H
#define SELECTION_EVALUATOR_H

#include <vector>
#include <cstdint>

#include "types/expected_requests.h"
#include "types/graph.h"

/**
 * A class used for calculating the total profit of a specific network configuration.
 * 
 * This class acts as the "inner loop" for both Genetic (Link-Based) and Brute Force solvers.
 * It simulates the routing of expected order requests through a subset of activated 
 * edges using the Successive Shortest Path Algorithm, while accounting for rate 
 * limits and path latency constraints to determine the total achievable profit.
 */
class SelectionEvaluator final {
public:
    /**
     * @param graph The network topology.
     * @param requests The set of order opportunity requests to be routed.
     * @param max_latency The maximum acceptable latency considered acceptable by the firm.
     */
    SelectionEvaluator(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency);

    /**
     * Calculates the total profit for a given set of activated links.
     * @param selected_edges A bit-packed vector where each bit represents the activation state of an edge.
     * @return The total profit achieved from successful order routing minus the leasing costs of the activated links.
     */
    double evaluate(const std::vector<std::uint64_t>& selected_edges);

    const std::vector<std::size_t>& get_used_edges() const;

private:
    /**
     * Attempts to route flow for a specific request through the active network.
     * Uses internal buffers to find the shortest feasible path and updates 
     * residual capacities.
     * @param request The specific expected order request to route.
     * @param selected_edges The current network configuration.
     * @param remaining_orders The number of orders that still need routing.
     * @return The number of orders successfully routed in this pass.
     */
    int update_path_flow(const HFT::Request& request, 
                         const std::vector<std::uint64_t>& selected_edges,
                         int remaining_orders);

    /**
     * Handles the fulfillment logic and capacity consumption for a request.
     */
    int process_orders(const HFT::Request& request, int remaining_orders);

    bool edge_is_selected(std::size_t edge_index, const std::vector<std::uint64_t>& selected_edges) const;

    // Pre-allocated buffers to avoid repeated heap allocations during evaluation.
    std::vector<double> m_min_latency_buffer;
    std::vector<const HFT::Edge*> m_parent_edge_buffer;
    std::vector<int> m_path_flow;
    std::vector<std::uint64_t> m_used_edges;

    const double m_max_latency{};
    const HFT::Graph& m_graph;
    const HFT::ExpectedRequests& m_requests;
};

#endif