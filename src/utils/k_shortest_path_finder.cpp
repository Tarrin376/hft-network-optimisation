#include "utils/k_shortest_path_finder.h"

#include <vector>
#include <cstdint>
#include <queue>
#include <limits>
#include <algorithm>
#include <utility>

#include "types/graph.h"
#include "types/state.h"
#include "utils/ksp_trie.h"

KShortestPathFinder::KShortestPathFinder(const HFT::Graph& graph)
    : m_globally_disabled_edges((graph.get_num_edges() / 64) + 1)
    , m_disabled_edges((graph.get_num_edges() / 64) + 1)
    , m_disabled_nodes((graph.get_num_nodes() / 64) + 1)
    , m_min_latency_buffer(graph.get_num_nodes(), { std::numeric_limits<double>::max(), 0 })
    , m_parent_edge_buffer(graph.get_num_nodes(), nullptr)
    , m_graph{ graph } {}

std::vector<KShortestPathFinder::Path>& KShortestPathFinder::find_paths(std::size_t source, std::size_t dest, std::uint32_t num_shortest_paths) {
    m_shortest_paths.clear();
    
    if (num_shortest_paths == 0) {
        return m_shortest_paths;
    }

    Path path{ dijkstra(source, dest) };
    m_shortest_paths.reserve(num_shortest_paths);
    m_ksp_trie.reset();

    if (path.edge_indices.empty()) {
        return m_shortest_paths;
    }

    std::priority_queue<Path, std::vector<Path>, std::greater<Path>> pq{};
    m_ksp_trie.insert(path.edge_indices);
    pq.push(std::move(path));

    for (int i = 0; i < num_shortest_paths && !pq.empty(); ++i) {
        m_shortest_paths.push_back(std::move(pq.top()));
        pq.pop();

        if (i == num_shortest_paths - 1) {
            break;
        }

        const std::vector<std::size_t>& edge_indices = m_shortest_paths.back().edge_indices;
        m_root_path_edges.reserve(edge_indices.size());
        m_dirty_nodes.reserve(edge_indices.size());

        double root_latency = 0;
        for (auto edge_id : edge_indices) {
            const auto& edge = m_graph.get_edge(edge_id);
            std::size_t spur_node = edge.source;

            disable_matching_outgoing_edges();
            Path spur_path = dijkstra(spur_node, dest);

            if (!spur_path.edge_indices.empty()) {
                m_path_buffer.edge_indices.clear();
                m_path_buffer.edge_indices.reserve(m_root_path_edges.size() + spur_path.edge_indices.size());
                m_path_buffer.total_latency = root_latency + spur_path.total_latency;
                
                m_path_buffer.edge_indices.assign(m_root_path_edges.begin(), m_root_path_edges.end());
                m_path_buffer.edge_indices.insert(m_path_buffer.edge_indices.end(), 
                                                spur_path.edge_indices.begin(), 
                                                spur_path.edge_indices.end());

                if (!m_ksp_trie.exists_exact_match(m_path_buffer.edge_indices)) {
                    m_ksp_trie.insert(m_path_buffer.edge_indices);
                    pq.push(std::move(m_path_buffer));
                }
            }

            m_root_path_edges.push_back(edge_id);
            root_latency += edge.latency;

            disable_node(spur_node);
            clear_disabled_edges();
        }

        clear_disabled_nodes();
        m_root_path_edges.clear();
    }

    return m_shortest_paths;
}

void KShortestPathFinder::globally_disable_edge(std::size_t edge_id) {
    std::size_t block = edge_id / 64;
    std::uint64_t mask = 1ULL << (edge_id % 64);
    
    if (!(m_globally_disabled_edges[block] & mask)) {
        m_globally_disabled_edges[block] |= mask;
        m_dirty_global_edges.push_back(edge_id);
    }
}

void KShortestPathFinder::clear_globally_disabled_edges() {
    for (std::size_t edge_id : m_dirty_global_edges) {
        m_globally_disabled_edges[edge_id / 64] &= ~(1ULL << (edge_id % 64));
    }

    m_dirty_global_edges.clear();
}

KShortestPathFinder::Path KShortestPathFinder::dijkstra(std::size_t source, std::size_t dest) {
    std::priority_queue<HFT::State, std::vector<HFT::State>, std::greater<HFT::State>> pq{};
    pq.push({0.0, source});

    m_min_latency_buffer[source] = { 0.0, ++m_latency_version };
    m_parent_edge_buffer[source] = nullptr;

    while (!pq.empty()) {
        const HFT::State current = pq.top();
        pq.pop();

        if (current.node_id == dest) {
            break;
        }

        if (current.latency > m_min_latency_buffer[current.node_id].first) {
            continue;
        }

        const auto& node = m_graph.get_node(current.node_id);
        for (const auto& edge_id : node.outgoing_edges) {
            const auto& edge = m_graph.get_edge(edge_id);

            if (edge_is_disabled(edge_id) || node_is_disabled(edge.dest)) {
                continue;
            }

            double new_latency = current.latency + edge.latency;
            const auto& next_pair = m_min_latency_buffer[edge.dest];

            double existing_latency = (next_pair.second == m_latency_version) 
                          ? next_pair.first 
                          : std::numeric_limits<double>::max();

            if (new_latency < existing_latency) {
                m_min_latency_buffer[edge.dest] = { new_latency, m_latency_version };
                m_parent_edge_buffer[edge.dest] = &edge;
                pq.emplace(new_latency, edge.dest);
            }
        }
    }

    if (m_min_latency_buffer[dest].second < m_latency_version) {
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
    const auto* outgoing_edges{ m_ksp_trie.find_matching_outgoing_edges(m_root_path_edges) };
    if (!outgoing_edges) {
        return;
    }

    for (const auto& edge : *outgoing_edges) {
        disable_edge(edge->edge_id);
    }
}

void KShortestPathFinder::disable_edge(std::size_t edge_id) {
    std::size_t block = edge_id / 64;
    std::uint64_t mask = 1ULL << (edge_id % 64);
    
    if (!(m_disabled_edges[block] & mask)) {
        m_disabled_edges[block] |= mask;
        m_dirty_edges.push_back(edge_id);
    }
}

void KShortestPathFinder::disable_node(std::size_t node_id) {
    std::size_t block = node_id / 64;
    std::uint64_t mask = 1ULL << (node_id % 64);

    if (!(m_disabled_nodes[block] & mask)) {
        m_disabled_nodes[block] |= mask;
        m_dirty_nodes.push_back(node_id);
    }
}

bool KShortestPathFinder::edge_is_disabled(std::size_t edge_id) {
    std::size_t block = edge_id / 64;
    std::uint64_t mask = 1ULL << (edge_id % 64);
    return (m_disabled_edges[block] | m_globally_disabled_edges[block]) & mask;
}

bool KShortestPathFinder::node_is_disabled(std::size_t node_id) {
    return m_disabled_nodes[node_id / 64] & (1ULL << (node_id % 64));
}

void KShortestPathFinder::clear_disabled_edges() {
    for (std::size_t edge_id : m_dirty_edges) {
        m_disabled_edges[edge_id / 64] &= ~(1ULL << (edge_id % 64));
    }

    m_dirty_edges.clear();
}

void KShortestPathFinder::clear_disabled_nodes() {
    for (std::size_t node_id : m_dirty_nodes) {
        m_disabled_nodes[node_id / 64] &= ~(1ULL << (node_id % 64));
    }

    m_dirty_nodes.clear();
}
