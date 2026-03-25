#include "utils/ksp_trie.h"

#include <algorithm>
#include <vector>
#include <deque>
#include <cstdint>

KSPTrie::KSPTrie() : m_root{ std::make_unique<KSPNode>() } {}

void KSPTrie::insert(const std::vector<std::size_t>& edge_indices) {
    KSPNode* cur{ m_root.get() };

    for (auto edge_id : edge_indices) {
        KSPNode* next = nullptr;

        for (const auto& child : cur->children) {
            if (child->edge_id == edge_id) {
                next = child.get();
                break;
            }
        }

        if (!next) {
            auto new_node = std::make_unique<KSPNode>();
            new_node->edge_id = edge_id;

            next = new_node.get();
            cur->children.push_back(std::move(new_node));
        }

        cur = next;
    }

    cur->is_leaf = true;
}

const std::deque<std::unique_ptr<KSPTrie::KSPNode>>* KSPTrie::find_matching_outgoing_edges(const std::deque<std::size_t>& root_path_edges) {
    KSPNode* cur{ m_root.get() };
    
    for (auto edge_id : root_path_edges) {
        const auto& children = cur->children;
        KSPNode* edge = get_matching_edge(children, edge_id);
        
        if (!edge) {
            return nullptr;
        }
        
        cur = edge;
    }
    
    return &cur->children;
}

bool KSPTrie::exists_exact_match(const std::vector<std::size_t>& edge_indices) {
    KSPNode* cur{ m_root.get() };
    
    for (auto edge_id : edge_indices) {
        const auto& children = cur->children;
        KSPNode* edge = get_matching_edge(children, edge_id);
        
        if (!edge) {
            return false;
        }
        
        cur = edge;
    }
    
    return cur->is_leaf;
} 

KSPTrie::KSPNode* KSPTrie::get_matching_edge(const std::deque<std::unique_ptr<KSPNode>>& children, std::size_t edge_id) {
    for (const auto& child : children) {
        if (child->edge_id == edge_id) {
            return child.get();
        }
    }
    
    return nullptr;
} 

void KSPTrie::reset() {
    m_root = std::make_unique<KSPNode>();
}
