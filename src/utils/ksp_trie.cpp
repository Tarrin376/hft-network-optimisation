#include "utils/ksp_trie.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstdint>

void KSPTrie::insert(const std::vector<std::size_t>& edge_indices) {
    KSPNode* cur{ &root };

    for (std::size_t i = 0; i < edge_indices.size(); ++i) {
        if (!cur->children.contains(edge_indices[i])) {
            cur->children.insert({ edge_indices[i], KSPNode{} });
        }

        cur = &cur->children.at(edge_indices[i]); 
        if (i == edge_indices.size() - 1) {
            cur->is_leaf = true;
        }
    }
}

std::vector<std::size_t> KSPTrie::find_matching_outgoing_edges(const std::vector<std::size_t>& edge_indices) {
    KSPNode* cur{ &root };

    for (int edge_id : edge_indices) {
        auto edge = cur->children.find(edge_id);
        if (edge == cur->children.end()) {
            return {};
        }

        cur = &edge->second;
    }
    
    std::vector<std::size_t> matching_outgoing_edges{};
    for (const auto& pair : cur->children) {
        matching_outgoing_edges.push_back(pair.first);
    }

    return matching_outgoing_edges;
}

bool KSPTrie::exists_exact_match(const std::vector<std::size_t>& edge_indices) {
    KSPNode* cur{ &root };

    for (int edge_id : edge_indices) {
        auto edge = cur->children.find(edge_id);
        if (edge == cur->children.end()) {
            return {};
        }

        cur = &edge->second;
    }

    return cur->is_leaf;
}

void KSPTrie::reset() {
    root = {};
}