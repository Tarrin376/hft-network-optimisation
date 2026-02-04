#include <vector>
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
        int remaining_orders{ request.num_orders };

        while (remaining_orders > 0) {
            int orders_sent{ get_max_order_flow(graph, request, remaining_orders) };
            if (orders_sent == 0) {
                return -1;
            }

            remaining_orders -= orders_sent;
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

int BruteForceSolver::get_max_order_flow(const Graph& graph, const Request& request, int remaining_orders) {
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
            return send_orders(request, parent_edge_buffer, remaining_orders);
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

    return 0;
}

int BruteForceSolver::send_orders(const Request& request, std::vector<const Edge*>& parent_edge_buffer, int remaining_orders) {
    const Edge* cur_edge{ parent_edge_buffer.at(request.exchange) };
    int min_rate_limit{ cur_edge->rate_limit };

    while (cur_edge) {
        min_rate_limit = std::min(min_rate_limit, cur_edge->rate_limit);
        cur_edge = parent_edge_buffer.at(cur_edge->source);
    }

    int orders_sent{ std::min(remaining_orders, min_rate_limit * request.planning_horizon) };
    cur_edge = parent_edge_buffer.at(request.exchange);

    while (cur_edge) {
        m_path_flow.at(cur_edge->id) += orders_sent;
        cur_edge = parent_edge_buffer.at(cur_edge->source);
    }

    return orders_sent;
}