#include <cstdint>
#include <iostream>

#include <benchmark/benchmark.h>

#include "utils/graph_generator.h"
#include "solvers/path_based_ga_solver.h"
#include "types/graph_gen_config.h"
#include "types/graph.h"

static void BM_GraphGen(benchmark::State& state) {
    for (auto _ : state) {
        HFT::GraphGenConfig config{
            .max_latency = 20.5,
            .max_rate_limit = 4,
            .max_lease_cost = 300,
            .num_nodes = 10000,
            .num_edges = 50000,
            .num_servers = 400
        };

        HFT::ExpectedRequests requests{};
        requests.push_back({ 
            .server = 8645, 
            .exchange = 233, 
            .num_orders = 4, 
            .planning_horizon = 13, 
            .max_order_profit = 56000 
        });

        GraphGenerator generator{ config };
        const HFT::Graph& graph{ generator.generate() };

        PathBasedGASolver solver{ graph, requests, {}, 20.5, 30 };
        double total_profit { solver.solve() };
        std::cout << "Total profit: " << total_profit << '\n';
    }
}

BENCHMARK(BM_GraphGen);

BENCHMARK_MAIN();