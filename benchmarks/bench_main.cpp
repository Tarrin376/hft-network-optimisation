#include <cstdint>
#include <iostream>

#include <benchmark/benchmark.h>

#include "utils/graph_generator.h"
#include "solvers/milp_solver.h"
#include "types/graph_gen_config.h"
#include "types/graph.h"

static void BM_GraphGen(benchmark::State& state) {
    for (auto _ : state) {
        HFT::GraphGenConfig config{
            .max_latency = 20.5,
            .max_rate_limit = 4,
            .max_lease_cost = 300,
            .num_nodes = 5000,
            .num_edges = 20000,
            .num_servers = 78
        };

        HFT::ExpectedRequests requests{};
        requests.push_back({ 
            .server = 34, 
            .exchange = 567, 
            .num_orders = 6, 
            .planning_horizon = 7, 
            .max_order_profit = 3000 
        });

        GraphGenerator generator{ config };
        const HFT::Graph& graph{ generator.generate() };

        MILPSolver solver{ graph, requests, { "CP-SAT" }, 20.5 };
        double total_profit { solver.solve() };
        std::cout << "Total profit: " << total_profit << '\n';
    }
}

BENCHMARK(BM_GraphGen);

BENCHMARK_MAIN();