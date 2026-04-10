#include "bench_utils/solver_fixture.h"

#include <cstdint>
#include <memory>
#include <random>

#include <benchmark/benchmark.h>

struct GraphState {
    std::int64_t num_nodes{};
    double density{};
};

void SolverFixture::SetUp(const ::benchmark::State& state) {
    const std::size_t num_nodes = static_cast<std::size_t>(state.range(0));
    const std::size_t num_edges = static_cast<std::size_t>(state.range(1));
    const std::size_t num_requests = static_cast<std::size_t>(state.range(2));
    
    std::mt19937 setup_gen(34);

    HFT::GraphGenConfig config{
        .max_latency = 20.5,
        .min_rate_limit = 5,
        .max_rate_limit = 20,
        .min_lease_cost = 100,
        .max_lease_cost = 1200,
        .min_order_profit = 100,
        .max_order_profit = 3000,
        .max_planning_horizon = 10,
        .num_orders = 15,
        .num_nodes = num_nodes,
        .num_edges = num_edges,
        .num_servers = std::max(1UL, static_cast<std::size_t>(num_nodes * 0.05)),
        .num_requests = num_requests,
        .seed = setup_gen()
    };

    generator = std::make_unique<GraphGenerator>(config);
}

void SolverFixture::TearDown(const ::benchmark::State& _) {
    // Do Nothing
}

void SolverFixture::ScalingArguments(benchmark::internal::Benchmark* b) {
    std::vector<GraphState> states{
        GraphState{ 70, 0.30 },
        GraphState{ 150, 0.20 },
        GraphState{ 250, 0.20 },
        GraphState{ 350, 0.20 },
        GraphState{ 450, 0.20 },
        GraphState{ 600, 0.15 },
        // GraphState{ 250, 0.10 },
        // GraphState{ 500, 0.10 },
        // GraphState{ 750, 0.05 },
        // GraphState{ 1000, 0.05 },
        // GraphState{ 1500, 0.05 },
        // GraphState{ 2000, 0.05 },
        // GraphState{ 500, 0.005 },
        // GraphState{ 1000, 0.005 },
        // GraphState{ 2500, 0.001 },
        // GraphState{ 5000, 0.001 },
        // GraphState{ 7000, 0.001 },
        // GraphState{ 15000, 0.0001 },
    };

    const int num_requests{ 20 };
    for (auto state : states) {
        std::int64_t edges = static_cast<std::int64_t>((state.num_nodes * (state.num_nodes - 1)) * state.density);
        b->Args({state.num_nodes, edges, num_requests});
    }
}
