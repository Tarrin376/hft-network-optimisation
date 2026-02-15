#include <cstdint>
#include <vector>
#include <queue>
#include <limits>
#include <iostream>

#include "utils/selection_evaluator.h"

#include "types/expected_requests.h"
#include "types/graph.h"

SelectionEvaluator::SelectionEvaluator(int max_order_profit, double max_latency) 
    : m_max_order_profit{ max_order_profit }
    , m_max_latency{ max_latency } {}

double SelectionEvaluator::evaluate(const HFT::Graph& graph, 
                                    const HFT::ExpectedRequests& requests,
                                    const std::vector<uint64_t>& selected_edges) const {
    auto total_profit{ find_total_profit(graph, requests, selected_edges) };
    return total_profit;
}

double SelectionEvaluator::find_total_profit(const HFT::Graph& graph, 
                                             const HFT::ExpectedRequests& requests, 
                                             const std::vector<uint64_t>& selected_edges) const {
    std::size_t num_edges{ graph.get_num_edges() };
    std::vector<int> path_flow(num_edges, 0);
    double total_profit{ 0 };

    for (const auto& request : requests) {
        int remaining_orders = request.num_orders;
        while (remaining_orders > 0) {
            int processed_orders = get_processed_orders(graph, request, selected_edges, path_flow, remaining_orders);
            if (processed_orders == 0) {
                return -(std::numeric_limits<double>::infinity());
            }
            
            remaining_orders -= processed_orders;
        }

        double request_profit = m_max_order_profit * request.num_orders;
        for (int i = 0; i < num_edges; ++i) {
            if (edge_is_selected(i, selected_edges)) {
                const auto& edge = graph.get_edge(i);
                double penalty = path_flow[i] * m_max_order_profit * (edge.latency / m_max_latency);
                request_profit -= penalty;
            }
        }

        path_flow.assign(num_edges, 0);
        total_profit += request_profit;
    }

    for (std::size_t i = 0; i < num_edges; ++i) {
        if (edge_is_selected(i, selected_edges)) {
            total_profit -= graph.get_edge(i).lease_cost;
        }
    }

    return total_profit;
}

int SelectionEvaluator::get_processed_orders(const HFT::Graph& graph, 
                                             const HFT::Request& request, 
                                             const std::vector<uint64_t>& selected_edges, 
                                             std::vector<int>& path_flow,
                                             int remaining_orders) const {
    struct State {
        double latency;
        std::size_t node_id;
    };

    auto cmp = [](const State& a, const State& b) {
        return a.latency > b.latency;
    };
    
    std::priority_queue<State, std::vector<State>, decltype(cmp)> pq{};
    std::size_t num_nodes{ graph.get_num_nodes() };

    std::vector<double> min_latency_buffer(num_nodes, std::numeric_limits<double>::max());
    std::vector<const HFT::Edge*> parent_edge_buffer(num_nodes, nullptr);

    min_latency_buffer[request.server] = 0;
    pq.push({0, request.server});

    while (!pq.empty()) {
        const State current = pq.top();
        pq.pop();

        if (current.node_id == request.exchange) {
            return process_orders(request, parent_edge_buffer, path_flow, remaining_orders);
        }

        if (current.latency > min_latency_buffer[current.node_id]) {
            continue;
        }

        const auto& node = graph.get_node(current.node_id);
        for (const auto& edge_index : node.outgoing_edges) {
            if (!edge_is_selected(edge_index, selected_edges)) {
                continue;
            }

            const auto& edge = graph.get_edge(edge_index);
            int current_flow = path_flow[edge_index];
            
            if (current_flow == edge.rate_limit * request.planning_horizon) {
                continue;
            }

            double new_latency = current.latency + edge.latency;
            if (new_latency < min_latency_buffer[edge.dest]) {
                min_latency_buffer[edge.dest] = new_latency;
                parent_edge_buffer[edge.dest] = &edge;
                pq.push({new_latency, edge.dest});
            }
        }
    }

    return 0;
}

int SelectionEvaluator::process_orders(const HFT::Request& request, 
                                       std::vector<const HFT::Edge*>& parent_edge_buffer,
                                       std::vector<int>& path_flow, 
                                       int remaining_orders) const {
    const HFT::Edge* cur_edge{ parent_edge_buffer[request.exchange] };
    int min_rate_limit{ cur_edge->rate_limit };

    while (cur_edge) {
        min_rate_limit = std::min(min_rate_limit, cur_edge->rate_limit);
        cur_edge = parent_edge_buffer[cur_edge->source];
    }

    int processed_orders{ std::min(remaining_orders, min_rate_limit * request.planning_horizon) };
    cur_edge = parent_edge_buffer[request.exchange];

    while (cur_edge) {
        path_flow[cur_edge->id] += processed_orders;
        cur_edge = parent_edge_buffer[cur_edge->source];
    }

    return processed_orders;
}

bool SelectionEvaluator::edge_is_selected(std::size_t edge_index, const std::vector<uint64_t>& selected_edges) const {
    return selected_edges[edge_index / 64] & (1ULL << (edge_index % 64));
}