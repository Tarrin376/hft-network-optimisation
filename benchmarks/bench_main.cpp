#include <cstdint>
#include <random>

#include <benchmark/benchmark.h>

#include "utils/graph_generator.h"
#include "solvers/path_based_ga_solver.h"
#include "types/graph_gen_config.h"
#include "types/expected_requests.h"
#include "types/graph.h"

static void BM_GASolver_Scaling(benchmark::State& state) {
    const std::size_t num_nodes{ static_cast<std::size_t>(state.range(0)) };
    const std::size_t num_edges{ static_cast<std::size_t>(state.range(1)) };
    const std::size_t num_requests{ static_cast<std::size_t>(state.range(2)) };
    const int num_shortest_paths{ static_cast<int>(state.range(3)) };

    std::mt19937 seed_gen(42);

    HFT::GraphGenConfig config{
        .max_latency = 20.5,
        .max_rate_limit = 4,
        .max_lease_cost = 300,
        .num_nodes = num_nodes,
        .num_edges = num_edges,
        .num_servers = num_nodes / 20,
        .num_requests = num_requests,
        .seed = seed_gen()
    };

    GraphGenerator generator{ config };
    generator.generate();

    double total_valid_profit = 0.0;
    std::uint64_t success_count = 0;

    for (auto _ : state) {
        PathBasedGASolver solver{ 
            generator.get_graph(),
            generator.get_requests(), 
            { .seed = seed_gen() }, 
            20.5,
            num_shortest_paths
        };

        double total_profit = solver.solve();
        benchmark::DoNotOptimize(total_profit);

        if (total_profit > std::numeric_limits<double>::lowest()) { 
            total_valid_profit += total_profit;
            success_count++;
        }

        benchmark::ClobberMemory();
    }

    if (success_count > 0) {
        state.counters["AvgValidProfit"] = total_valid_profit / success_count;
    } else {
        state.counters["AvgValidProfit"] = -std::numeric_limits<double>::infinity();
    }

    state.counters["SuccessRate"] = static_cast<double>(success_count) / state.iterations();
}

BENCHMARK(BM_GASolver_Scaling)
    ->Args({500, 35000, 13, 45})
    ->Args({10000, 300000, 2, 10})
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();