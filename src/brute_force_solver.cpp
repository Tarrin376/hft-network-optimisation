#include <vector>
#include <functional>
#include <utility>
#include <queue>
#include <limits>
#include <cstdint>
#include <iostream>

#include "brute_force_solver.h"
#include "order_opportunity.h"
#include "graph.h"
#include "solver.h"

BruteForceSolver::BruteForceSolver(int max_order_profit, int max_latency) 
: Solver{ max_order_profit, max_latency } {}

int BruteForceSolver::solve(const Graph& graph, const std::vector<OrderOpportunity>& opportunities) {
    std::size_t num_edges{ graph.get_num_edges() };
    m_selected_edges.assign(num_edges, false); 

    auto max_profit{ find_max_profit(graph, opportunities, 0) };
    return max_profit;
}

int BruteForceSolver::find_max_profit(const Graph& graph,
                                      const std::vector<OrderOpportunity>& opportunities,
                                      std::size_t index) {
    if (index == graph.get_num_edges()) {
        return calculate_total_profit(graph, opportunities);
    }

    m_selected_edges[index] = 1;
    auto select_edge_profit{ find_max_profit(graph, opportunities, index + 1) };

    m_selected_edges[index] = 0;
    auto ignore_edge_profit{ find_max_profit(graph, opportunities, index + 1) };

    return std::max(select_edge_profit, ignore_edge_profit);
}

int BruteForceSolver::calculate_total_profit(const Graph& graph, const std::vector<OrderOpportunity>& opportunities) {
    for (const auto& opportunity : opportunities) {
        m_path_flow.assign(graph.get_num_edges(), 0);
        for (int i = 0; i < opportunity.num_orders; ++i) {
            bool found_path = find_optimal_path(graph, opportunity);
            if (!found_path) {
                return 0;
            }
        }
    }

    int total_profit{ 0 };
    for (const int edge_id : m_selected_edges) {
        if (m_selected_edges[edge_id] == 1) {
            const Edge& edge{ graph.get_edge(edge_id) };
            int gross_profit{ m_max_order_profit * std::max(1 - edge.latency / m_max_latency, 0) };
            total_profit += gross_profit * m_path_flow.at(edge.id) - edge.lease_cost;
        }
    }

    return total_profit;
}

bool BruteForceSolver::find_optimal_path(const Graph& graph, const OrderOpportunity& opportunity) {
    struct State {
        int latency;
        std::size_t node_id;
        bool operator>(const State& other) const { return latency > other.latency; }
    };
    
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq{};
    std::size_t num_nodes{ graph.get_num_nodes() };

    std::vector<int> min_latency_buffer(num_nodes, std::numeric_limits<int>::max());
    std::vector<const Edge*> parent_edge_buffer(num_nodes, nullptr);

    min_latency_buffer[opportunity.server] = 0;
    pq.push({0, opportunity.server});

    while (!pq.empty()) {
        const State current = pq.top();
        pq.pop();

        if (current.node_id == opportunity.exchange) {
            update_flow_path(opportunity.exchange, parent_edge_buffer);
            return true;
        }

        if (current.latency > min_latency_buffer[current.node_id]) {
            continue;
        }

        for (const auto& edge_id : graph.get_node(current.node_id).edges) {
            if (m_selected_edges[edge_id] == 0) {
                continue;
            }

            const Edge& edge = graph.get_edge(edge_id);
            int current_flow = m_path_flow.at(edge.id);
            
            if (current_flow == edge.rate_limit * opportunity.planning_horizon) {
                continue;
            }

            int new_latency = current.latency + edge.latency;
            if (new_latency < min_latency_buffer[edge.dest]) {
                min_latency_buffer[edge.dest] = new_latency;
                parent_edge_buffer[edge.dest] = &edge;
                pq.push({new_latency, edge.dest});
            }
        }
    }

    return false;
}

void BruteForceSolver::update_flow_path(int target_exchange, const std::vector<const Edge*>& parent_edge_buffer) {
    const Edge* cur_edge{ parent_edge_buffer.at(target_exchange) };

    while (cur_edge) {
        m_path_flow.at(cur_edge->id)++;
        cur_edge = parent_edge_buffer.at(cur_edge->source);
    }
}