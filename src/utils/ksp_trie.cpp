#include "utils/ksp_trie.h"

#include <algorithm>
#include <vector>
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

std::vector<std::size_t> KSPTrie::find_matching_outgoing_edges(const std::vector<std::size_t>& edge_indices) {
    KSPNode* cur{ m_root.get() };
    
    for (auto edge_id : edge_indices) {
        const auto& children = cur->children;
        KSPNode* edge = get_matching_edge(children, edge_id);
        
        if (!edge) {
            return {};
        }
        
        cur = edge;
    } 
    
    const auto& children = cur->children;
    std::vector<std::size_t> matching_outgoing_edges(children.size());
    
    std::transform(children.begin(), children.end(), matching_outgoing_edges.begin(),
        [](const std::unique_ptr<KSPNode>& node) {
            return node->edge_id;
        });
    
    return matching_outgoing_edges;
}

bool KSPTrie::exists_exact_match(const std::vector<std::size_t>& edge_indices) {
    KSPNode* cur{ m_root.get() };
    
    for (auto edge_id : edge_indices) {
        const auto& children = cur->children;
        KSPNode* edge = get_matching_edge(children, edge_id);
        
        if (!edge) {
            return {};
        }
        
        cur = edge;
    }
    
    return cur->is_leaf;
} 

KSPTrie::KSPNode* KSPTrie::get_matching_edge(const std::vector<std::unique_ptr<KSPNode>>& children, std::size_t edge_id) {
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