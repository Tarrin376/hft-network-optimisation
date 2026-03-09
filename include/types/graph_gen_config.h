#ifndef GRAPH_GENERATOR_CONFIG_H
#define GRAPH_GENERATOR_CONFIG_H

#include <cstdint>

namespace HFT {
    struct GraphGenConfig {
        double server_density{};
        double max_latency{};
        int max_rate_limit{};
        int max_lease_cost{};
        std::size_t num_nodes{};
        std::size_t num_edges{};
        unsigned long long seed{ 34ULL };
    };
}

#endif