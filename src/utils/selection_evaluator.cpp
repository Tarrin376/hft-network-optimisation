#include "utils/selection_evaluator.h"

#include <functional>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <queue>
#include <limits>

#include "types/expected_requests.h"
#include "types/state.h"
#include "types/graph.h"

SelectionEvaluator::SelectionEvaluator(double max_latency, const HFT::Graph& graph, const HFT::ExpectedRequests& requests) 
: m_min_latency_buffer(graph.get_num_nodes(), std::numeric_limits<double>::max())
, m_parent_edge_buffer(graph.get_num_nodes(), nullptr)
, m_path_flow(graph.get_num_edges(), 0)
, m_max_latency{ max_latency }
, m_graph{ graph }
, m_requests{ requests } {}

void SelectionEvaluator::reset() {
    std::fill(m_min_latency_buffer.begin(), m_min_latency_buffer.end(), std::numeric_limits<double>::max());
    std::fill(m_parent_edge_buffer.begin(), m_parent_edge_buffer.end(), nullptr);
}

double SelectionEvaluator::evaluate(const std::vector<std::uint64_t>& selected_edges) {
    std::fill(m_path_flow.begin(), m_path_flow.end(), 0);

    std::size_t num_edges{ m_graph.get_num_edges() };
    double total_profit{ 0 };

    for (const auto& request : m_requests) {
        int remaining_orders = request.num_orders;
        while (remaining_orders > 0) {
            int processed_orders = update_path_flow(request, selected_edges, remaining_orders);
            reset();
            
            if (processed_orders == 0) {
                return std::numeric_limits<double>::lowest();
            }
            
            remaining_orders -= processed_orders;
        }

        double request_profit = request.max_order_profit * request.num_orders;
        for (int i = 0; i < num_edges; ++i) {
            if (edge_is_selected(i, selected_edges)) {
                const auto& edge = m_graph.get_edge(i);
                double penalty = m_path_flow[i] * request.max_order_profit * (edge.latency / m_max_latency);
                request_profit -= penalty;
            }
        }

        total_profit += request_profit;
        std::fill(m_path_flow.begin(), m_path_flow.end(), 0);
    }

    for (std::size_t i = 0; i < num_edges; ++i) {
        if (edge_is_selected(i, selected_edges)) {
            total_profit -= m_graph.get_edge(i).lease_cost;
        }
    }

    return total_profit;
}

int SelectionEvaluator::update_path_flow(const HFT::Request& request, 
                                         const std::vector<std::uint64_t>& selected_edges, 
                                         int remaining_orders) {
    std::priority_queue<HFT::State, std::vector<HFT::State>, std::greater<HFT::State>> pq{};
    m_min_latency_buffer[request.server] = 0;
    pq.push({0, request.server});

    while (!pq.empty()) {
        const HFT::State current = pq.top();
        pq.pop();

        if (current.node_id == request.exchange) {
            return process_orders(request, remaining_orders);
        }

        if (current.latency > m_min_latency_buffer[current.node_id]) {
            continue;
        }

        const auto& node = m_graph.get_node(current.node_id);
        for (const auto& edge_index : node.outgoing_edges) {
            if (!edge_is_selected(edge_index, selected_edges)) {
                continue;
            }

            const auto& edge = m_graph.get_edge(edge_index);
            int current_flow = m_path_flow[edge_index];
            
            if (current_flow == edge.rate_limit * request.planning_horizon) {
                continue;
            }

            double new_latency = current.latency + edge.latency;
            if (new_latency < m_min_latency_buffer[edge.dest]) {
                m_min_latency_buffer[edge.dest] = new_latency;
                m_parent_edge_buffer[edge.dest] = &edge;
                pq.emplace(new_latency, edge.dest);
            }
        }
    }

    return 0;
}

int SelectionEvaluator::process_orders(const HFT::Request& request, int remaining_orders) {
    const HFT::Edge* cur_edge{ m_parent_edge_buffer[request.exchange] };
    int min_rate_limit{ cur_edge->rate_limit };

    while (cur_edge) {
        min_rate_limit = std::min(min_rate_limit, cur_edge->rate_limit);
        cur_edge = m_parent_edge_buffer[cur_edge->source];
    }

    int processed_orders{ std::min(remaining_orders, min_rate_limit * request.planning_horizon) };
    cur_edge = m_parent_edge_buffer[request.exchange];

    while (cur_edge) {
        m_path_flow[cur_edge->id] += processed_orders;
        cur_edge = m_parent_edge_buffer[cur_edge->source];
    }

    return processed_orders;
}

bool SelectionEvaluator::edge_is_selected(std::size_t edge_index, const std::vector<std::uint64_t>& selected_edges) const {
    return selected_edges[edge_index / 64] & (1ULL << (edge_index % 64));
}