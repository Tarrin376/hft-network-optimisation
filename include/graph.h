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

struct Node {
    int id{};
    bool is_server{};
    std::vector<int> edges{};
};

class Graph {
public:
    void add_edge(const Edge& edge, bool source_is_server);
    const Edge& get_edge(int edgeId);
    const Node& get_node(int nodeId);
    const std::vector<int>& outgoing(int node) const;
    int num_nodes() const;

private:
    std::unordered_map<int, Edge> m_edges{};
    std::unordered_map<int, Node> m_nodes{};
};

#endif