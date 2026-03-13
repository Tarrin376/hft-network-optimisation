#ifndef K_SHORTEST_PATH_FINDER
#define K_SHORTEST_PATH_FINDER

#include <memory>
#include <cstdint>
#include <vector>
#include <unordered_set>

#include "types/graph.h"

class KShortestPathFinder {
public:
    struct Path {
        std::vector<std::size_t> edge_indices{};
        double total_latency{};
        
        bool operator>(const Path& other) const {
            return total_latency > other.total_latency;
        }

        bool operator<(const Path& other) const {
            return total_latency < other.total_latency;
        }
    };

    KShortestPathFinder(const HFT::Graph& graph);

    std::vector<Path> find_paths(std::size_t source, std::size_t dest, int k);

private:
    const HFT::Graph& m_graph;
    std::unordered_set<std::size_t> m_disabled_edges;
    std::unordered_set<std::size_t> m_disabled_nodes;

    std::vector<double> m_min_latency_buffer;
    std::vector<const HFT::Edge*> m_parent_edge_buffer;

    Path dijkstra(std::size_t source, std::size_t dest);

    void disable_edges(const std::vector<Path>& confirmed_paths, 
                       const std::vector<std::size_t>& root_edges);
};

#endif