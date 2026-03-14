#include "utils/k_shortest_path_finder.h"

#include <vector>
#include <cstdint>
#include <queue>
#include <limits>
#include <algorithm>

#include "types/graph.h"
#include "types/state.h"
#include "utils/ksp_trie.h"

KShortestPathFinder::KShortestPathFinder(const HFT::Graph& graph)
    : m_disabled_edges((graph.get_num_edges() / 64) + 1)
    , m_disabled_nodes((graph.get_num_nodes() / 64) + 1)
    , m_min_latency_buffer(graph.get_num_nodes(), std::numeric_limits<double>::max())
    , m_parent_edge_buffer(graph.get_num_nodes(), nullptr)
    , m_graph{ graph } {}

std::vector<KShortestPathFinder::Path>& KShortestPathFinder::find_paths(std::size_t source, std::size_t dest, int k) {
    Path path{ dijkstra(source, dest) };
    m_k_shortest_paths.clear();

    if (path.edge_indices.empty()) {
        return m_k_shortest_paths;
    }

    std::priority_queue<Path, std::vector<Path>, std::greater<Path>> pq{};
    pq.push(std::move(path));
    m_ksp_trie.reset();

    for (int i = 0; i < k && !pq.empty(); ++i) {
        while (pq.size() > 0) {
            if (m_ksp_trie.exists_exact_match(pq.top().edge_indices)) pq.pop();
            else break;
        }

        if (i == k - 1 || pq.empty()) {
            break;
        }

        m_k_shortest_paths.push_back(std::move(pq.top()));
        m_ksp_trie.insert(m_k_shortest_paths[i].edge_indices);
        pq.pop();

        double root_latency = 0;
        for (auto edge_id : m_k_shortest_paths[i].edge_indices) {
            const auto& edge = m_graph.get_edge(edge_id);
            std::size_t spur_node = edge.source;

            disable_matching_outgoing_edges();
            Path spur_path = dijkstra(spur_node, dest);

            if (!spur_path.edge_indices.empty()) {
                Path total_path = {};
                total_path.edge_indices = m_root_path_edges;
                total_path.edge_indices.insert(total_path.edge_indices.end(), spur_path.edge_indices.begin(), spur_path.edge_indices.end());
                total_path.total_latency = root_latency + spur_path.total_latency;

                pq.push(std::move(total_path));
            }

            m_root_path_edges.push_back(edge_id);
            root_latency += edge.latency;

            m_disabled_nodes[spur_node / 64] |= (1ULL << (spur_node % 64));
            m_disabled_edges.assign(m_disabled_edges.size(), 0);
        }

        m_disabled_nodes.assign(m_disabled_edges.size(), 0);
        m_root_path_edges.clear();
    }

    return m_k_shortest_paths;
}

KShortestPathFinder::Path KShortestPathFinder::dijkstra(std::size_t source, std::size_t dest) {
    m_min_latency_buffer.assign(m_graph.get_num_nodes(), std::numeric_limits<double>::max());
    m_parent_edge_buffer.assign(m_graph.get_num_nodes(), nullptr);

    std::priority_queue<HFT::State, std::vector<HFT::State>, std::greater<HFT::State>> pq{};
    m_min_latency_buffer[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        const HFT::State current = pq.top();
        pq.pop();

        if (current.node_id == dest) {
            break;
        }

        if (current.latency > m_min_latency_buffer[current.node_id]) {
            continue;
        }

        const auto& node = m_graph.get_node(current.node_id);
        for (const auto& edge_id : node.outgoing_edges) {
            const auto& edge = m_graph.get_edge(edge_id);
            bool edge_disabled = m_disabled_edges[edge_id / 64] & (1ULL << (edge_id % 64));
            bool node_disabled = m_disabled_nodes[edge.dest / 64] & (1ULL << (edge.dest % 64));

            if (edge_disabled || node_disabled) {
                continue;
            }

            double new_latency = current.latency + edge.latency;
            if (new_latency < m_min_latency_buffer[edge.dest]) {
                m_min_latency_buffer[edge.dest] = new_latency;
                m_parent_edge_buffer[edge.dest] = &edge;
                pq.emplace(new_latency, edge.dest);
            }
        }
    }

    if (!m_parent_edge_buffer[dest]) {
        return {};
    }

    const HFT::Edge* cur_edge{ m_parent_edge_buffer[dest] };
    Path path{};

    while (cur_edge) {
        path.edge_indices.push_back(cur_edge->id);
        path.total_latency += cur_edge->latency;
        cur_edge = m_parent_edge_buffer[cur_edge->source];
    }

    std::reverse(path.edge_indices.begin(), path.edge_indices.end());
    return path;
}

void KShortestPathFinder::disable_matching_outgoing_edges() {
    std::vector<std::size_t> outgoing_edges{ m_ksp_trie.find_matching_outgoing_edges(m_root_path_edges) };
    for (auto edge : outgoing_edges) {
        m_disabled_edges[edge / 64] |= (1ULL << (edge % 64));
    }
}