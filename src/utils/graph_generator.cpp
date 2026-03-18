#include "utils/graph_generator.h"

#include <cstdint>
#include <cassert>
#include <set>

#include "types/graph_gen_config.h"
#include "types/graph.h"

GraphGenerator::GraphGenerator(const HFT::GraphGenConfig& config) 
: m_rate_limit_dist(0, config.max_rate_limit)
, m_lease_cost_dist(1, config.max_lease_cost)
, m_latency_dist(0, config.max_latency)
, m_node_dist(0, config.num_nodes - 1)
, m_config{ config }
, m_gen{ config.seed }
, m_graph{ config.num_nodes, config.num_edges } {
    assert(config.num_edges <= (config.num_nodes * (config.num_nodes - 1)) && "Too many edges supplied.");
}

void GraphGenerator::generate() {
    reset();
    generate_nodes();
    assign_servers();
    generate_edges();
    generate_requests();
}

const HFT::Graph& GraphGenerator::get_graph() const {
    return m_graph;
}

const HFT::ExpectedRequests& GraphGenerator::get_requests() const {
    return m_requests;
}

const HFT::GraphGenConfig& GraphGenerator::get_config() const {
    return m_config;
}

void GraphGenerator::reset() {
    m_graph.reset();
    m_requests.clear();
    m_used_edges.clear();
}

void GraphGenerator::generate_requests() {
    for (std::size_t i = 0; i < m_config.num_requests; ++i) {
        std::size_t server = static_cast<std::size_t>(m_node_dist(m_gen));
        while (!m_graph.get_node(server).is_server) {
            server = static_cast<std::size_t>(m_node_dist(m_gen));
        }

        std::size_t exchange = static_cast<std::size_t>(m_node_dist(m_gen));
        while (m_graph.get_node(exchange).is_server) {
            exchange = static_cast<std::size_t>(m_node_dist(m_gen));
        }

        HFT::Request new_request{ 
            .server = server, 
            .exchange = exchange, 
            .num_orders = 5, 
            .planning_horizon = 10, 
            .max_order_profit = 4000
        };

        m_requests.push_back(new_request);
    }
}

void GraphGenerator::generate_nodes() {
    for (std::size_t i = 0; i < m_graph.get_num_nodes(); ++i) {
        m_graph.add_node({ i, false });
    }
}

void GraphGenerator::generate_edges() {
    std::size_t edge_id{ 0 };
    
    while (edge_id < m_config.num_edges) {
        std::size_t from = static_cast<std::size_t>(m_node_dist(m_gen));
        std::size_t to = static_cast<std::size_t>(m_node_dist(m_gen));

        if (from != to && m_used_edges.find({from, to}) == m_used_edges.end()) {
            m_used_edges.insert({from, to});
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

    m_graph.add_edge(edge);
}

void GraphGenerator::assign_servers() {
    std::size_t remaining_servers{ m_config.num_servers };

    while (remaining_servers > 0) {
        std::size_t node_id = static_cast<std::size_t>(m_node_dist(m_gen));
        if (!m_graph.get_node(node_id).is_server) {
            m_graph.add_node({ node_id, true });
            remaining_servers--;
        }
    }
}