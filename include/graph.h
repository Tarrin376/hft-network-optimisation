#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>

struct Edge {
    int id{};
    int source{};
    int dest{};
    int rate_limit{};
    int latency{};
    int lease_cost{};
};

struct Node {
    int id{};
    bool is_server{};
    std::vector<int> edges{};
};

class Graph {
public:
    void add_edge(const Edge& edge, bool source_is_server);

    const Edge& get_edge(int edge_id) const;
    const Node& get_node(int node_id) const;

    const std::vector<int>& outgoing(int node) const;
    
    const std::vector<int> get_edge_ids() const;
    const std::vector<int> get_node_ids() const;

private:
    std::unordered_map<int, Edge> m_edges{};
    std::unordered_map<int, Node> m_nodes{};
};

#endif