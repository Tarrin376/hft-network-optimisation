#ifndef KSP_TRIE_H
#define KSP_TRIE_H

#include <unordered_map>
#include <vector>
#include <cstdint>

class KSPTrie {
    public:
        struct KSPNode {
            bool is_leaf{};
            std::unordered_map<std::size_t, KSPNode> children{};
        };

        void insert(const std::vector<std::size_t>& edge_indices);

        std::vector<std::size_t> find_matching_outgoing_edges(const std::vector<std::size_t>& edge_indices);

        bool exists_exact_match(const std::vector<std::size_t>& edge_indices);

        void reset();
    
    private:
        KSPNode root{};
};

#endif