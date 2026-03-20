#ifndef KSP_TRIE_H
#define KSP_TRIE_H

#include <vector>
#include <cstdint>
#include <memory>

class KSPTrie {
    public:
        struct KSPNode {
            bool is_leaf{};
            std::size_t edge_id{};
            std::vector<std::unique_ptr<KSPNode>> children{};
        };

        KSPTrie();

        void insert(const std::vector<std::size_t>& edge_indices);

        std::vector<std::size_t> find_matching_outgoing_edges(const std::vector<std::size_t>& edge_indices);

        bool exists_exact_match(const std::vector<std::size_t>& edge_indices);

        KSPNode* get_matching_edge(const std::vector<std::unique_ptr<KSPNode>>& children, std::size_t edge_id);

        void reset();
    
    private:
        std::unique_ptr<KSPNode> m_root;
};

#endif