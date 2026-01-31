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
    void add_edge(const Edge& edge);
    const Edge& get_edge(int edgeId);
    const std::vector<int>& outgoing(int node) const;
    int num_nodes() const;

private:
    std::unordered_map<int, std::vector<int>> m_adj_list{};
    std::unordered_map<int, Edge> m_edges{};
};

#endif