#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <cstdint>

struct Edge {
    std::size_t id{};
    std::size_t source{};
    std::size_t dest{};
    int rate_limit{};
    int latency{};
    int lease_cost{};
};

struct Node {
    std::size_t id{};
    bool is_server{};
    std::vector<std::size_t> edges{};
};

class Graph {
public:
    void add_edge(const Edge& edge, bool source_is_server);

    const Edge& get_edge(std::size_t edge_id) const;
    const Node& get_node(std::size_t node_id) const;

    const std::vector<std::size_t>& outgoing(std::size_t node) const;

    const std::size_t get_num_nodes() const;
    const std::size_t get_num_edges() const;
    
    const std::vector<std::size_t> get_edge_ids() const;
    const std::vector<std::size_t> get_node_ids() const;

private:
    std::unordered_map<std::size_t, Edge> m_edges{};
    std::unordered_map<std::size_t, Node> m_nodes{};
};

#endif