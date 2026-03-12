#ifndef K_SHORTEST_PATH_FINDER
#define K_SHORTEST_PATH_FINDER

#include <memory>
#include <cstdint>
#include <vector>
#include <set>

#include "types/graph.h"

class KShortestPathFinder {
public:
    struct Path {
        std::vector<std::size_t> edge_indices{};
        double total_latency{};
        
        bool operator>(const Path& other) const {
            return total_latency > other.total_latency;
        }
    };

    KShortestPathFinder(const HFT::Graph& graph);

    std::vector<std::shared_ptr<Path>> find_paths(std::size_t source, std::size_t dest, int k);

private:
    const HFT::Graph& m_graph;
    std::set<std::size_t> m_disabled_edges;
    std::set<std::size_t> m_disabled_nodes;

    std::shared_ptr<Path> dijkstra(std::size_t source, std::size_t dest);

    void disable_edges(const std::vector<std::shared_ptr<Path>>& confirmed_paths, 
                       const std::vector<std::size_t>& root_edges);
};

#endif