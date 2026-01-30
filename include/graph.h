#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <cstdint>

struct Edge {
    int id{};
    int source{};
    int dest{};
    int rate_limit{};
    double latency{};
    std::uint64_t lease_cost{};
};

class Graph {
public:
    void addEdge(const Edge& edge);
    const Edge& getEdge(int edgeId);
    const std::vector<int>& outgoingEdges(int node) const;
    int numNodes() const;

private:
    std::unordered_map<int, std::vector<int>> m_adjList{};
    std::unordered_map<int, Edge> m_edges{};
};

#endif