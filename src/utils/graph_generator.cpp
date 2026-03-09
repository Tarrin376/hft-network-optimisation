#include <memory>
#include <cstdint>
#include <random>
#include <set>

#include "utils/graph_generator.h"

#include "types/graph_gen_config.h"
#include "types/graph.h"

GraphGenerator::GraphGenerator(const HFT::GraphGenConfig& config) 
    : m_config{ config }
    , m_graph{ config.num_nodes, config.num_edges } {}

HFT::Graph& GraphGenerator::generate() {
    m_graph.reset();
    generate_edges();
    return m_graph;
}

void GraphGenerator::generate_edges() {
    std::set<std::pair<std::size_t, std::size_t>> used{};
    std::uniform_int_distribution<int> node_dist(0, m_config.num_nodes - 1);
    std::size_t edge_id{ 0 };
    
    while (edge_id < m_config.num_edges) {
        std::size_t from = node_dist(m_gen);
        std::size_t to = node_dist(m_gen);

        if (from != to && used.find({from, to}) == used.end()) {
            used.insert({from, to});
            create_edge(from, to, edge_id);
            edge_id++;
        }
    }
}

void GraphGenerator::create_edge(std::size_t from, std::size_t to, std::size_t edge_id) {
    std::uniform_int_distribution<int> rate_limit_dist(0, m_config.max_rate_limit);
    std::uniform_int_distribution<int> lease_cost_dist(0, m_config.max_lease_cost);
    std::uniform_real_distribution<double> latency_dist(0, m_config.max_latency);

    HFT::Edge edge{ 
        edge_id, 
        from, 
        to, 
        rate_limit_dist(m_gen), 
        latency_dist(m_gen), 
        lease_cost_dist(m_gen) 
    };

    m_graph.add_edge(edge, false);
}