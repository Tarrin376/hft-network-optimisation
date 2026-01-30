#include <unordered_map>
#include <vector>
#include <cassert>
#include "graph.h"

void Graph::addEdge(const Edge& edge) {
    if (!m_adjList.contains(edge.source)) {
        m_adjList[edge.source] = {};
    }

    if (!m_adjList.contains(edge.dest)) {
        m_adjList[edge.dest] = {};
    }

    m_adjList[edge.source].push_back(edge.id);
    m_edges[edge.id] = edge;
}

const Edge& Graph::getEdge(int edgeId) {
    assert(m_edges.contains(edgeId));
    return m_edges.at(edgeId);
}

const std::vector<int>& Graph::outgoingEdges(int node) const {
    assert(m_adjList.contains(node));
    return m_adjList.at(node);
}

int Graph::numNodes() const {
    return static_cast<int>(m_adjList.size());
}