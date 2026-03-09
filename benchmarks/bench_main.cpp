#include <cstdint>
#include <benchmark/benchmark.h>

#include "utils/graph_generator.h"

#include "types/graph_gen_config.h"
#include "types/graph.h"

static void BM_GraphGen(benchmark::State& state) {
    for (auto _ : state) {
        HFT::GraphGenConfig config{
            .server_density = 0.01,
            .max_latency = 20.5,
            .max_rate_limit = 4,
            .max_lease_cost = 3000,
            .num_nodes = 10,
            .num_edges = 5
        };

        GraphGenerator generator{ config };
        HFT::Graph& graph{ generator.generate() };
    }
}

BENCHMARK(BM_GraphGen);

BENCHMARK_MAIN();