#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <utility>
#include <optional>
#include <queue>
#include <limits>

#include "brute_force_solver.h"
#include "order_opportunity.h"
#include "graph.h"
#include "solver.h"

BruteForceSolver::BruteForceSolver(int max_order_profit, int max_latency) 
: Solver{ max_order_profit, max_latency } {}

int BruteForceSolver::solve(const Graph& graph, const std::vector<OrderOpportunity>& opportunities) {
    auto& edge_ids{ graph.get_edge_ids() };
    std::unordered_set<int> selected_edges{};

    auto max_profit{ find_max_profit(graph, edge_ids, opportunities, selected_edges, 0) };
    return max_profit;
}

int BruteForceSolver::find_max_profit(const Graph& graph,
                                      const std::vector<int> edge_ids,
                                      const std::vector<OrderOpportunity>& opportunities,
                                      std::unordered_set<int>& selected_edges,  
                                      int index) {
    if (index == edge_ids.size()) {
        return calculate_total_profit(graph, selected_edges, opportunities);
    }

    selected_edges.insert(edge_ids.at(index));
    auto select_edge_profit{ find_max_profit(graph, edge_ids, opportunities, selected_edges, index + 1) };

    selected_edges.erase(edge_ids.at(index));
    auto ignore_edge_profit{ find_max_profit(graph, edge_ids, opportunities, selected_edges, index + 1) };

    return std::max(select_edge_profit, ignore_edge_profit);
}

int BruteForceSolver::calculate_total_profit(const Graph& graph,
                                             const std::unordered_set<int>& selected_edges, 
                                             const std::vector<OrderOpportunity>& opportunities) {
    std::unordered_map<int, int> path_flow{};
    int total_profit{ 0 };

    for (const auto& opportunity : opportunities) {
        for (int i = 0; i < opportunity.num_orders; ++i) {
            bool found_path = find_optimal_path(graph, opportunity, selected_edges, path_flow);
            if (!found_path) {
                return 0;
            }
        }
    }

    for (const int edge_id : selected_edges) {
        const Edge& edge{ graph.get_edge(edge_id) };
        int net_profit{ m_max_order_profit * std::max(1 - edge.latency / m_max_latency, 0) };
        total_profit += path_flow[edge.id] * net_profit - edge.lease_cost;
    }

    return total_profit;
}

bool BruteForceSolver::find_optimal_path(const Graph& graph,
                                         const OrderOpportunity& opportunity,   
                                         const std::unordered_set<int>& selected_edges,
                                         std::unordered_map<int, int>& path_flow) {
    std::unordered_map<int, BruteForceSolver::LatencyPair> latency_costs{};
    std::vector<int> node_ids{ graph.get_node_ids() };

    for (const auto node : node_ids) {
        latency_costs.insert({ node, { std::numeric_limits<int>::max(), nullptr }});
    }

    latency_costs[opportunity.server].latency = 0;

    auto compare = [](std::pair<int, int>& a, std::pair<int, int>& b) { 
        return a.first > b.first; 
    };

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(compare)> pq{};
    pq.push({ 0, opportunity.server });

    while (!pq.empty()) {
        const auto& current{ pq.top() };
        const int latency{ current.first };
        const int node_id{ current.second };

        if (node_id == opportunity.exchange) {
            update_flow_path(latency_costs, path_flow, opportunity.exchange);
            return true;
        }

        if (latency > latency_costs.at(node_id).latency) {
            continue;
        }

        const Node& node{ graph.get_node(node_id) };

        for (const auto& edge_id : node.edges) {
            const Edge& edge{ graph.get_edge(edge_id) };
            if (!selected_edges.contains(edge.id)) {
                continue;
            }

            auto rate_limit_satisfied = [&path_flow, &opportunity, &edge]() {
                return path_flow[edge.id] <= edge.rate_limit * opportunity.planning_horizon;
            };
            
            BruteForceSolver::LatencyPair& latency_pair{ latency_costs.at(edge.dest) };
            int updated_latency{ latency + edge.latency };

            if (updated_latency < latency_pair.latency && rate_limit_satisfied()) {
                pq.push({ updated_latency, edge.dest });
                latency_pair.latency = updated_latency;
                latency_pair.edge_ptr = &edge;
            }
        }

        pq.pop();
    }

    return false;
}

void BruteForceSolver::update_flow_path(const std::unordered_map<int, BruteForceSolver::LatencyPair> latency_costs, 
                                        std::unordered_map<int, int>& path_flow, 
                                        int dest) {
    int cur_node{ dest };
    const Edge* cur_edge{ latency_costs.at(dest).edge_ptr };

    while (cur_edge) {
        path_flow[cur_edge->id]++;
        cur_node = cur_edge->source;
        cur_edge = latency_costs.at(cur_edge->source).edge_ptr;
    }
}