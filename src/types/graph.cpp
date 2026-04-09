#include "types/graph.h"

#include <vector>
#include <cassert>
#include <cstddef>

HFT::Graph::Graph(std::size_t num_nodes, std::size_t num_edges) 
    : m_nodes(num_nodes)
    , m_edges(num_edges)
    , m_num_nodes{ num_nodes }
    , m_num_edges{ num_edges } 
{}

void HFT::Graph::add_edge(const Edge& edge) {
    assert(edge.source < m_num_nodes);
    assert(edge.dest < m_num_nodes);
    assert(edge.id < m_num_edges);

    m_edges[edge.id] = edge;

    m_nodes[edge.source].outgoing_edges.push_back(edge.id);
    m_nodes[edge.dest].incoming_edges.push_back(edge.id);
}

void HFT::Graph::add_node(const Node& node) {
    assert(node.id < m_num_nodes);
    m_nodes[node.id] = node;
}

const HFT::Edge& HFT::Graph::get_edge(std::size_t edge_id) const {
    assert(edge_id < m_num_edges);
    return m_edges[edge_id];
}

const HFT::Node& HFT::Graph::get_node(std::size_t node_id) const {
    assert(node_id < m_num_nodes);
    return m_nodes[node_id];
}

std::size_t HFT::Graph::get_num_nodes() const {
    return m_num_nodes;
}

std::size_t HFT::Graph::get_num_edges() const {
    return m_num_edges;
}