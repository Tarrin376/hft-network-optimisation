#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <cstdint>
#include <random>
#include <unordered_set>

#include "types/graph_gen_config.h"
#include "types/graph.h"

class GraphGenerator {
public:
    GraphGenerator(const HFT::GraphGenConfig& config);

    const HFT::Graph& generate();

    void reset();

private:
    void generate_edges();
    void create_edge(std::size_t from, std::size_t to, std::size_t edge_id);
    void assign_servers();

    std::unordered_set<std::size_t> m_servers{};
    HFT::Graph m_graph;

    HFT::GraphGenConfig m_config{};
    std::mt19937 m_gen{};
};

#endif