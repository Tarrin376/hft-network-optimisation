#include <vector>
#include <cassert>

#include "graph.h"

void Graph::add_edge(const Edge& edge) {
    if (!m_adj_list.contains(edge.source)) {
        m_adj_list[edge.source] = {};
    }

    if (!m_adj_list.contains(edge.dest)) {
        m_adj_list[edge.dest] = {};
    }

    m_adj_list[edge.source].push_back(edge.id);
    m_edges[edge.id] = edge;
}

const Edge& Graph::get_edge(int edgeId) {
    assert(m_edges.contains(edgeId));
    return m_edges.at(edgeId);
}

const std::vector<int>& Graph::outgoing(int node) const {
    assert(m_adj_list.contains(node));
    return m_adj_list.at(node);
}

int Graph::num_nodes() const {
    return static_cast<int>(m_adj_list.size());
}