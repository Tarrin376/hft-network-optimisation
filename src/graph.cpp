#include <vector>
#include <cassert>

#include "graph.h"

void Graph::add_edge(const Edge& edge, bool source_is_server) {
    if (!m_nodes.contains(edge.source)) {
        m_nodes[edge.source] = Node{ .id = edge.source, .is_server = source_is_server };
    }

    if (!m_nodes.contains(edge.dest)) {
        m_nodes[edge.dest] = Node{ .id = edge.dest };
    }

    m_nodes[edge.source].edges.push_back(edge.id);
    m_edges[edge.id] = edge;
}

const Edge& Graph::get_edge(int edgeId) {
    assert(m_edges.contains(edgeId));
    return m_edges.at(edgeId);
}

const Node& Graph::get_node(int nodeId) {
    assert(m_nodes.contains(nodeId));
    return m_nodes.at(nodeId);
}

const std::vector<int>& Graph::outgoing(int node) const {
    assert(m_nodes.contains(node));
    return m_nodes.at(node).edges;
}

int Graph::num_nodes() const {
    return static_cast<int>(m_nodes.size());
}