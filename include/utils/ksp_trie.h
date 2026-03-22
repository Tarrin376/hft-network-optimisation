#ifndef KSP_TRIE_H
#define KSP_TRIE_H

#include <deque>
#include <vector>
#include <cstdint>
#include <memory>

class KSPTrie {
    public:
        struct KSPNode {
            bool is_leaf{};
            std::size_t edge_id{};
            std::deque<std::unique_ptr<KSPNode>> children{};
        };

        KSPTrie();

        void insert(const std::vector<std::size_t>& edge_indices);

        const std::deque<std::unique_ptr<KSPTrie::KSPNode>>* find_matching_outgoing_edges(const std::deque<std::size_t>& root_path_edges);

        bool exists_exact_match(const std::vector<std::size_t>& edge_indices);

        KSPNode* get_matching_edge(const std::deque<std::unique_ptr<KSPNode>>& children, std::size_t edge_id);

        void reset();
    
    private:
        std::unique_ptr<KSPNode> m_root;
};

#endif