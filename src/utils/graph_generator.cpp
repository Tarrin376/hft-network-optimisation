#include "utils/graph_generator.h"

#include <cstdint>
#include <cassert>
#include <set>

#include "types/graph_gen_config.h"
#include "types/graph.h"

GraphGenerator::GraphGenerator(const HFT::GraphGenConfig& config) 
: m_rate_limit_dist(config.min_rate_limit, config.max_rate_limit)
, m_lease_cost_dist(config.min_lease_cost, config.max_lease_cost)
, m_latency_dist(0, config.max_latency)
, m_node_dist(0, config.num_nodes - 1)
, m_planning_horizon_dist(1, config.max_planning_horizon)
, m_order_profit_dist(config.min_order_profit, config.max_order_profit)
, m_config{ config }
, m_gen{ config.seed }
, m_graph{ config.num_nodes, config.num_edges } {
    generate();
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

void GraphGenerator::generate() {
    generate_nodes();
    assign_servers();
    generate_edges();
    generate_requests();
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
            .num_orders = m_config.num_orders, 
            .planning_horizon = m_planning_horizon_dist(m_gen), 
            .max_order_profit = m_order_profit_dist(m_gen)
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

        if (is_valid_edge(from, to)) {
            m_used_edges.insert({from, to});
            create_edge(from, to, edge_id);
            edge_id++;
        }
    }
}

bool GraphGenerator::is_valid_edge(std::size_t from, std::size_t to) {
    if (from == to) {
        return false;
    }

    const auto& source = m_graph.get_node(from);
    const auto& dest = m_graph.get_node(to);

    if ((source.is_server && dest.is_server) || (!source.is_server && dest.is_server)) {
        return false;
    }

    return m_used_edges.find({from, to}) == m_used_edges.end();
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