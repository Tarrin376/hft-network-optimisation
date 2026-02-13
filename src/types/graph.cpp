#include <vector>
#include <cassert>
#include <cstddef>

#include "types/graph.h"

Graph::Graph(std::size_t num_nodes, std::size_t num_edges) 
    : m_nodes(num_nodes)
    , m_edges(num_edges)
    , m_num_nodes{ num_nodes }
    , m_num_edges{ num_edges } 
{}

void Graph::add_edge(const Edge& edge, bool source_is_server) {
    assert(edge.source < m_num_nodes);
    assert(edge.dest < m_num_nodes);
    assert(edge.id < m_num_edges);

    m_edges[edge.id] = edge;

    auto& src = m_nodes[edge.source];
    auto& dst = m_nodes[edge.dest];

    src.id = edge.source;
    src.is_server = source_is_server;
    src.edges.push_back(edge.id);

    dst.id = edge.dest;
}

const Edge& Graph::get_edge(std::size_t edge_id) const {
    assert(edge_id < m_num_edges);
    return m_edges[edge_id];
}

const Node& Graph::get_node(std::size_t node_id) const {
    assert(node_id < m_num_nodes);
    return m_nodes[node_id];
}

const std::vector<std::size_t>& Graph::outgoing(std::size_t node_id) const {
    assert(node_id < m_num_nodes);
    return m_nodes[node_id].edges;
}

std::size_t Graph::get_num_nodes() const {
    return m_num_nodes;
}

std::size_t Graph::get_num_edges() const {
    return m_num_edges;
}