#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <cstddef>

#include "graph.h"

void Graph::add_edge(const Edge& edge, bool source_is_server) {
    if (!m_nodes.contains(edge.source)) {
        m_nodes.insert({ edge.source, Node{ .id = edge.source, .is_server = source_is_server } });
    }

    if (!m_nodes.contains(edge.dest)) {
        m_nodes.insert({ edge.dest, Node{ .id = edge.dest }});
    }

    m_nodes[edge.source].edges.push_back(edge.id);
    m_edges.insert({ edge.id, edge });
}

const Edge& Graph::get_edge(std::size_t edge_id) const {
    assert(m_edges.contains(edge_id));
    return m_edges.at(edge_id);
}

const Node& Graph::get_node(std::size_t node_id) const {
    assert(m_nodes.contains(node_id));
    return m_nodes.at(node_id);
}

const std::vector<std::size_t>& Graph::outgoing(std::size_t node) const {
    assert(m_nodes.contains(node));
    return m_nodes.at(node).edges;
}

const std::size_t Graph::get_num_nodes() const {
    return m_nodes.size();
}

const std::size_t Graph::get_num_edges() const {
    return m_edges.size();
}

const std::vector<std::size_t> Graph::get_edge_ids() const {
    std::vector<std::size_t> edge_ids{};
    
    std::transform(m_edges.begin(), m_edges.end(), back_inserter(edge_ids), 
        [](const std::pair<int, Edge>& pair) { 
            return pair.first; 
        });
    
    return edge_ids;
}

const std::vector<std::size_t> Graph::get_node_ids() const {
    std::vector<std::size_t> node_ids{};
    
    std::transform(m_nodes.begin(), m_nodes.end(), back_inserter(node_ids), 
        [](const std::pair<int, Node>& pair) { 
            return pair.first;
        });
    
    return node_ids;
}