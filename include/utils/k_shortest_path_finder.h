#ifndef K_SHORTEST_PATH_FINDER
#define K_SHORTEST_PATH_FINDER

#include <cstdint>
#include <vector>

#include "types/graph.h"
#include "utils/ksp_trie.h"

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

    std::vector<Path>& find_paths(std::size_t source, std::size_t dest, int k);

private:
    KSPTrie m_ksp_trie{};
    
    std::vector<std::uint64_t> m_disabled_edges;
    std::vector<std::uint64_t> m_disabled_nodes;

    std::vector<std::pair<double, std::uint32_t>> m_min_latency_buffer;
    std::vector<const HFT::Edge*> m_parent_edge_buffer;
    const HFT::Graph& m_graph;

    std::uint32_t m_latency_version{};
    Path m_path_buffer{};

    std::vector<Path> m_shortest_paths{};
    std::vector<std::size_t> m_root_path_edges{};

    std::vector<std::size_t> m_dirty_edges{};
    std::vector<std::size_t> m_dirty_nodes{};

    Path dijkstra(std::size_t source, std::size_t dest);

    void disable_matching_outgoing_edges();

    void disable_edge(std::size_t edge_id);
    void disable_node(std::size_t node_id);

    bool edge_is_disabled(std::size_t edge_id);
    bool node_is_disabled(std::size_t node_id);

    void clear_disabled_edges();
    void clear_disabled_nodes();
};

#endif