#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

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

const Edge& Graph::get_edge(int edge_id) const {
    assert(m_edges.contains(edge_id));
    return m_edges.at(edge_id);
}

const Node& Graph::get_node(int node_id) const {
    assert(m_nodes.contains(node_id));
    return m_nodes.at(node_id);
}

const std::vector<int>& Graph::outgoing(int node) const {
    assert(m_nodes.contains(node));
    return m_nodes.at(node).edges;
}

const std::vector<int> Graph::get_edge_ids() const {
    std::vector<int> edge_ids{};
    
    std::transform(m_edges.begin(), m_edges.end(), back_inserter(edge_ids), 
        [](const std::pair<int, Edge>& pair) { 
            return pair.first; 
        });
    
    return edge_ids;
}

const std::vector<int> Graph::get_node_ids() const {
    std::vector<int> node_ids{};
    
    std::transform(m_nodes.begin(), m_nodes.end(), back_inserter(node_ids), 
        [](const std::pair<int, Node>& pair) { 
            return pair.first;
        });
    
    return node_ids;
}