#ifndef KSP_TRIE_H
#define KSP_TRIE_H

#include <vector>
#include <cstdint>
#include <memory>

/**
 * A prefix tree (Trie) structure used for storing and querying path prefixes.
 * 
 * In the context of K-Shortest Path Algorithm, this trie 
 * is used to efficiently track paths that have already been explored or 
 * to identify deviations from a root path.
 */
class KSPTrie {
    public:
        struct KSPNode {
            bool is_leaf{};
            std::size_t edge_id{};
            std::vector<std::unique_ptr<KSPNode>> children{};
        };

        KSPTrie();

        void insert(const std::vector<std::size_t>& edge_indices);

        /**
         * Retrieves all children of the node representing the end of a root path.
         * This is primarily used to identify edges that should be temporarily 
         * disabled during Yen's Algorithm to force path branching.
         * @param root_path_edges The sequence of edges to navigate through the trie.
         * @return A pointer to the vector of children nodes, or nullptr if the 
         *         root path does not exist in the trie.
         */
        const std::vector<std::unique_ptr<KSPTrie::KSPNode>>* find_matching_outgoing_edges(const std::vector<std::size_t>& root_path_edges);

        /**
         * Checks if a specific path exists in the Trie.
         * @param edge_indices The sequence of edges to look up.
         * @return True if an exact match exists.
         */
        bool exists_exact_match(const std::vector<std::size_t>& edge_indices);

        /**
         * Performs a linear search on a node's children to find a specific edge.
         * @param children The list of potential branch nodes.
         * @param edge_id The specific edge ID to locate.
         * @return A pointer to the matching KSPNode, or nullptr if not found.
         */
        KSPNode* get_matching_edge(const std::vector<std::unique_ptr<KSPNode>>& children, std::size_t edge_id);

        void reset();
    
    private:
        std::unique_ptr<KSPNode> m_root;
};

#endif