#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <memory>
#include <random>
#include <cstdint>
#include <vector>

#include "types/graph_gen_config.h"
#include "types/graph.h"

class GraphGenerator {
public:
    GraphGenerator(const HFT::GraphGenConfig& config);

    HFT::Graph& generate();

private:
    void generate_edges();
    void create_edge(std::size_t from, std::size_t to, std::size_t edge_id);

    HFT::GraphGenConfig m_config{};
    HFT::Graph m_graph{ 0, 0 };
    std::mt19937 m_gen{};
};

#endif