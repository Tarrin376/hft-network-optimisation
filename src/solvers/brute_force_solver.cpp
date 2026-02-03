#include <vector>
#include <functional>
#include <utility>
#include <queue>
#include <limits>
#include <cstdint>
#include <iostream>

#include "solvers/brute_force_solver.h"
#include "types/expected_requests.h"
#include "types/graph.h"
#include "solvers/solver.h"

BruteForceSolver::BruteForceSolver(int max_order_profit, double max_latency) 
: Solver{ max_order_profit, max_latency } {}

double BruteForceSolver::solve(const Graph& graph, const ExpectedRequests& requests) {
    std::size_t num_edges{ graph.get_num_edges() };
    m_selected_edges.assign(num_edges, false); 

    auto max_profit{ find_max_profit(graph, requests, 0) };
    return max_profit;
}

double BruteForceSolver::find_max_profit(const Graph& graph, const ExpectedRequests& requests, std::size_t index) {
    if (index == graph.get_num_edges()) {
        return calculate_total_profit(graph, requests);
    }

    m_selected_edges[index] = true;
    auto select_edge_profit{ find_max_profit(graph, requests, index + 1) };

    m_selected_edges[index] = false;
    auto ignore_edge_profit{ find_max_profit(graph, requests, index + 1) };

    return std::max(select_edge_profit, ignore_edge_profit);
}

double BruteForceSolver::calculate_total_profit(const Graph& graph, const ExpectedRequests& requests) {
    for (const auto& request : requests) {
        m_path_flow.assign(graph.get_num_edges(), 0);
        for (int i = 0; i < request.num_orders; ++i) {
            bool found_path = find_optimal_path(graph, request);
            if (!found_path) {
                return 0;
            }
        }
    }

    double total_profit{ 0 };
    for (int i = 0; i < graph.get_num_edges(); ++i) {
        if (m_selected_edges[i]) {
            const auto& edge{ graph.get_edge(i) };
            double gross_profit{ m_max_order_profit * (1 - edge.latency / m_max_latency) };
            total_profit += gross_profit * m_path_flow.at(i) - edge.lease_cost;
        }
    }

    return total_profit;
}

bool BruteForceSolver::find_optimal_path(const Graph& graph, const Request& request) {
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
    std::vector<const Edge*> parent_edge_buffer(num_nodes, nullptr);

    min_latency_buffer[request.server] = 0;
    pq.push({0, request.server});

    while (!pq.empty()) {
        const State current = pq.top();
        pq.pop();

        if (current.node_id == request.exchange) {
            update_flow_path(request.exchange, parent_edge_buffer);
            return true;
        }

        if (current.latency > min_latency_buffer[current.node_id]) {
            continue;
        }

        for (const auto& edge_id : graph.get_node(current.node_id).edges) {
            if (!m_selected_edges[edge_id]) {
                continue;
            }

            const Edge& edge = graph.get_edge(edge_id);
            int current_flow = m_path_flow.at(edge.id);
            
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

    return false;
}

void BruteForceSolver::update_flow_path(int target_exchange, const std::vector<const Edge*>& parent_edge_buffer) {
    const Edge* cur_edge{ parent_edge_buffer.at(target_exchange) };

    while (cur_edge) {
        m_path_flow.at(cur_edge->id)++;
        cur_edge = parent_edge_buffer.at(cur_edge->source);
    }
}