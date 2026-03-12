#include "utils/k_shortest_path_finder.h"

#include <vector>
#include <cstdint>
#include <queue>
#include <limits>
#include <algorithm>
#include <functional>
#include <iostream>

#include "types/graph.h"
#include "types/state.h"

KShortestPathFinder::KShortestPathFinder(const HFT::Graph& graph)
    : m_graph{ graph } {}

std::vector<KShortestPathFinder::Path> KShortestPathFinder::find_paths(std::size_t source, std::size_t dest, int k) {
    Path path{ dijkstra(source, dest) };
    if (path.edge_indices.empty()) {
        return {};
    }

    std::priority_queue<Path, std::vector<Path>, std::greater<Path>> pq{};
    std::vector<Path> confirmed_paths{};
    pq.push(std::move(path));

    for (int i = 0; i < k && !pq.empty(); ++i) {
        while (pq.size() > 0) {
            const auto& top_edges = pq.top().edge_indices;
            bool is_new = std::none_of(confirmed_paths.begin(), confirmed_paths.end(),
                [&top_edges](const auto& p) {
                    return p.edge_indices == top_edges;
                });

            if (is_new) break;
            else pq.pop();
        }

        if (i == k - 1 || pq.empty()) {
            break;
        }

        std::vector<std::size_t> root_path_edges{};
        double root_latency = 0;

        confirmed_paths.push_back(std::move(pq.top()));
        pq.pop();

        for (auto edge_id : confirmed_paths[i].edge_indices) {
            auto& edge = m_graph.get_edge(edge_id);
            std::size_t spur_node = edge.source;

            disable_edges(confirmed_paths, root_path_edges);
            Path spur_path = dijkstra(spur_node, dest);

            if (!spur_path.edge_indices.empty()) {
                Path total_path = {};
                total_path.edge_indices = root_path_edges;
                total_path.edge_indices.insert(total_path.edge_indices.end(), spur_path.edge_indices.begin(), spur_path.edge_indices.end());
                total_path.total_latency = root_latency + spur_path.total_latency;

                pq.push(std::move(total_path));
            }

            root_path_edges.push_back(edge_id);
            m_disabled_nodes.insert(m_graph.get_edge(edge_id).source);
            root_latency += edge.latency;

            m_disabled_edges.clear();
        }

        m_disabled_nodes.clear();
    }

    return confirmed_paths;
}

KShortestPathFinder::Path KShortestPathFinder::dijkstra(std::size_t source, std::size_t dest) {
    std::priority_queue<HFT::State, std::vector<HFT::State>, std::greater<HFT::State>> pq{};
    std::vector<const HFT::Edge*> parent_edge_id_buffer(m_graph.get_num_nodes(), nullptr);
    std::vector<double> min_latency_buffer(m_graph.get_num_nodes(), std::numeric_limits<double>::max());

    min_latency_buffer[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        const HFT::State current = pq.top();
        pq.pop();

        if (current.node_id == dest) {
            break;
        }

        if (current.latency > min_latency_buffer[current.node_id]) {
            continue;
        }

        const auto& node = m_graph.get_node(current.node_id);
        for (const auto& edge_id : node.outgoing_edges) {
            const auto& edge = m_graph.get_edge(edge_id);
            if (m_disabled_edges.contains(edge_id) || m_disabled_nodes.contains(edge.dest)) {
                continue;
            }

            double new_latency = current.latency + edge.latency;
            if (new_latency < min_latency_buffer[edge.dest]) {
                min_latency_buffer[edge.dest] = new_latency;
                parent_edge_id_buffer[edge.dest] = &edge;
                pq.emplace(new_latency, edge.dest);
            }
        }
    }

    if (!parent_edge_id_buffer[dest]) {
        return {};
    }

    const HFT::Edge* cur_edge{ parent_edge_id_buffer[dest] };
    Path path{};

    while (cur_edge) {
        path.edge_indices.push_back(cur_edge->id);
        path.total_latency += cur_edge->latency;
        cur_edge = parent_edge_id_buffer[cur_edge->source];
    }

    std::reverse(path.edge_indices.begin(), path.edge_indices.end());
    return path;
}

void KShortestPathFinder::disable_edges(const std::vector<Path>& confirmed_paths, const std::vector<std::size_t>& root_edges) {
    for (const auto& path : confirmed_paths) {
        const auto& edges = path.edge_indices;
        if (edges.size() <= root_edges.size()) {
            continue;
        }

        bool match = true;
        for (std::size_t j = 0; j < root_edges.size(); ++j) {
            if (edges[j] != root_edges[j]) {
                match = false;
                break;
            }
        }

        if (match) {
            m_disabled_edges.insert(edges[root_edges.size()]);
        }
    }                              
}