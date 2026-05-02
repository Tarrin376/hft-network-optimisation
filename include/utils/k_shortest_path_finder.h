#ifndef K_SHORTEST_PATH_FINDER
#define K_SHORTEST_PATH_FINDER

#include <cstdint>
#include <cmath>
#include <vector>

#include "types/graph.h"
#include "utils/ksp_trie.h"

/**
 * An implementation of Yen's algorithm to find the top-K shortest paths between 
 * nodes.
 * 
 * This class identifies candidate paths prioritised by total latency. In cases 
 * of equivalent latency, it uses lease cost as a tie-breaker. It maintains 
 * internal buffers and a trie structure to optimise the search and prevent 
 * redundant path processing.
 */
class KShortestPathFinder {
public:
    /**
     * Represents a single candidate route through the network.
     */
    struct Path {
        std::vector<std::size_t> edge_indices{};
        double total_latency{};
        double total_lease_cost{};
        
        /**
         * Compares paths for priority queue ordering (min-heap behavior).
         * Prioritises lower latency. If latencies are within an epsilon range, 
         * the path with the lower lease cost is preferred.
         */
        bool operator>(const Path& other) const {
            constexpr double epsilon = 1e-9;

            if (std::abs(total_latency - other.total_latency) < epsilon) {
                return total_lease_cost > other.total_lease_cost;
            }
            
            return total_latency > other.total_latency;
        }
    };

    KShortestPathFinder(const HFT::Graph& graph);

    /**
     * Computes up to K shortest paths between a source and destination.
     * @param source The starting node.
     * @param dest The target node.
     * @param num_shortest_paths The maximum number of paths (K) to return.
     * @return A reference to the internal vector 'm_shortest_paths' containing the found paths.
     */
    std::vector<Path>& find_paths(std::size_t source, std::size_t dest, std::int32_t num_shortest_paths);

    /**
     * Marks an edge as unusable for all subsequent pathfinding operations.
     * Useful for modeling permanent link failures or restricted paths.
     */
    void globally_disable_edge(std::size_t edge_id);

    /**
     * Re-enables all edges previously restricted by globally_disable_edge.
     */
    void clear_globally_disabled_edges();

private:
    /**
     * Performs a single Dijkstra pass to find the shortest path between two nodes 
     * while respecting currently disabled nodes and edges.
     */
    Path dijkstra(std::size_t source, std::size_t dest);

    // Internal helper methods for constraint management.
    void disable_matching_outgoing_edges();
    void disable_edge(std::size_t edge_id);
    void disable_node(std::size_t node_id);
    bool edge_is_disabled(std::size_t edge_id);
    bool node_is_disabled(std::size_t node_id);
    void clear_disabled_edges();
    void clear_disabled_nodes();

    // Trie for storing the current shortest paths found.
    KSPTrie m_ksp_trie{};

    // Bitsets/vectors for tracking disabled components to avoid allocations.
    std::vector<std::uint64_t> m_globally_disabled_edges;
    std::vector<std::uint64_t> m_disabled_edges;
    std::vector<std::uint64_t> m_disabled_nodes;

    // Performance buffers to minimise heap allocations during search.
    std::vector<std::pair<double, std::uint32_t>> m_min_latency_buffer;
    std::vector<const HFT::Edge*> m_parent_edge_buffer;

    const HFT::Graph& m_graph;

    // Versioning and temporary buffers for current search state.
    std::uint32_t m_latency_version{};
    Path m_path_buffer{};
    std::vector<Path> m_shortest_paths{};
    std::vector<std::size_t> m_root_path_edges{};

    // Tracking for "dirty" indices to allow for efficient clear operations.
    std::vector<std::size_t> m_dirty_global_edges{};
    std::vector<std::size_t> m_dirty_edges{};
    std::vector<std::size_t> m_dirty_nodes{};
};

#endif