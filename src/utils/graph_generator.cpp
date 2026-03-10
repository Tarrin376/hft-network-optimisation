#include "utils/graph_generator.h"

#include <unordered_set>
#include <cstdint>
#include <random>
#include <cassert>
#include <set>

#include "types/graph_gen_config.h"
#include "types/graph.h"

GraphGenerator::GraphGenerator(const HFT::GraphGenConfig& config) 
: m_rate_limit_dist(0, config.max_rate_limit)
, m_lease_cost_dist(0, config.max_lease_cost)
, m_latency_dist(0, config.max_latency)
, m_node_dist(0, config.num_nodes - 1)
, m_config{ config }
, m_gen{ config.seed }
, m_graph{ config.num_nodes, config.num_edges } {
    assert(config.num_edges <= (config.num_nodes * (config.num_nodes - 1)) && "Too many edges supplied.");
}

const HFT::Graph& GraphGenerator::generate() {
    reset();
    assign_servers();
    generate_edges();
    return m_graph;
}

void GraphGenerator::reset() {
    m_graph.reset();
    m_servers.clear();
}

void GraphGenerator::generate_edges() {
    std::set<std::pair<std::size_t, std::size_t>> used_edges{};
    std::size_t edge_id{ 0 };
    
    while (edge_id < m_config.num_edges) {
        std::size_t from = static_cast<std::size_t>(m_node_dist(m_gen));
        std::size_t to = static_cast<std::size_t>(m_node_dist(m_gen));

        if (from != to && used_edges.find({from, to}) == used_edges.end()) {
            used_edges.insert({from, to});
            create_edge(from, to, edge_id);
            edge_id++;
        }
    }
}

void GraphGenerator::create_edge(std::size_t from, std::size_t to, std::size_t edge_id) {
    HFT::Edge edge{ 
        edge_id, 
        from, 
        to, 
        m_rate_limit_dist(m_gen), 
        m_latency_dist(m_gen), 
        m_lease_cost_dist(m_gen) 
    };

    m_graph.add_edge(edge, m_servers.contains(from));
}

void GraphGenerator::assign_servers() {
    while (m_servers.size() < m_config.num_servers) {
        std::size_t node_id = static_cast<std::size_t>(m_node_dist(m_gen));
        if (!m_servers.contains(node_id)) {
            m_servers.insert(node_id);
        }
    }
}