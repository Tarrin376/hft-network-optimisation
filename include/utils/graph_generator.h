#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <cstdint>
#include <random>
#include <set>

#include "types/graph_gen_config.h"
#include "types/expected_requests.h"
#include "types/graph.h"

class GraphGenerator {
public:
    GraphGenerator(const HFT::GraphGenConfig& config);

    const HFT::Graph& get_graph() const;
    const HFT::ExpectedRequests& get_requests() const;
    const HFT::GraphGenConfig& get_config() const;

private:
    void generate();
    void generate_requests();
    void generate_nodes();
    void generate_edges();

    bool is_valid_edge(std::size_t from, std::size_t to);
    void create_edge(std::size_t from, std::size_t to, std::size_t edge_id);
    void assign_servers();

    std::uniform_int_distribution<int> m_rate_limit_dist;
    std::uniform_int_distribution<int> m_lease_cost_dist;
    std::uniform_real_distribution<double> m_latency_dist;
    std::uniform_int_distribution<int> m_node_dist;
    std::uniform_int_distribution<int> m_num_orders_dist;
    std::uniform_int_distribution<int> m_planning_horizon_dist;
    std::uniform_int_distribution<int> m_order_profit_dist;

    HFT::GraphGenConfig m_config{};
    std::mt19937 m_gen{};
    HFT::Graph m_graph;

    std::set<std::pair<std::size_t, std::size_t>> m_used_edges{};
    HFT::ExpectedRequests m_requests{};
};

#endif