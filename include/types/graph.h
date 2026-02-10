#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <cstdint>

struct Edge {
    std::size_t id{};
    std::size_t source{};
    std::size_t dest{};
    int rate_limit{};
    double latency{};
    int lease_cost{};
};

struct Node {
    std::size_t id{};
    bool is_server{};
    std::vector<std::size_t> edges{};
};

class Graph {
public:
    Graph(std::size_t num_nodes, std::size_t num_edges);

    void add_edge(const Edge& edge, bool source_is_server);

    const Edge& get_edge(std::size_t edge_id) const;
    const Node& get_node(std::size_t node_id) const;

    const std::vector<std::size_t>& outgoing(std::size_t node_id) const;

    std::size_t get_num_nodes() const;
    std::size_t get_num_edges() const;

private:
    std::vector<Edge> m_edges{};
    std::vector<Node> m_nodes{};

    const std::size_t m_num_nodes{};
    const std::size_t m_num_edges{};
};

#endif