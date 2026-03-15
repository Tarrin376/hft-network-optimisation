#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <cstdint>
#include <random>
#include <set>
#include <unordered_set>

#include "types/graph_gen_config.h"
#include "types/expected_requests.h"
#include "types/graph.h"

class GraphGenerator {
public:
    GraphGenerator(const HFT::GraphGenConfig& config);

    void generate();

    const HFT::Graph& get_graph();
    const HFT::ExpectedRequests& get_requests();

private:
    void reset();
    void generate_requests();
    void generate_edges();
    void create_edge(std::size_t from, std::size_t to, std::size_t edge_id);
    void assign_servers();

    std::uniform_int_distribution<int> m_rate_limit_dist;
    std::uniform_int_distribution<int> m_lease_cost_dist;
    std::uniform_real_distribution<double> m_latency_dist;
    std::uniform_int_distribution<int> m_node_dist;

    std::set<std::pair<std::size_t, std::size_t>> m_used_edges{};
    std::unordered_set<std::size_t> m_servers{};

    HFT::ExpectedRequests m_requests{};
    HFT::Graph m_graph;

    HFT::GraphGenConfig m_config{};
    std::mt19937 m_gen{};
};

#endif