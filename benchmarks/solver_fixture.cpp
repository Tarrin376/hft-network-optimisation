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
    
    std::mt19937 setup_gen(89);

    HFT::GraphGenConfig config{
        .max_latency = 20.5,
        .min_rate_limit = 5,
        .max_rate_limit = 50,
        .min_lease_cost = 200,
        .max_lease_cost = 1000,
        .max_num_orders = 50,
        .min_order_profit = 500,
        .max_order_profit = 2000,
        .max_planning_horizon = 10,
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
        GraphState{ 20, 0.60 },
        GraphState{ 50, 0.50 },
        GraphState{ 100, 0.50 },
        GraphState{ 200, 0.40 },
        GraphState{ 300, 0.40 },
        GraphState{ 400, 0.40 },
        GraphState{ 500, 0.40 },
        GraphState{ 100, 0.15 },
        GraphState{ 250, 0.15 },
        GraphState{ 500, 0.15 },
        GraphState{ 600, 0.10 },
        GraphState{ 700, 0.10 },
        GraphState{ 850, 0.05 },
        GraphState{ 1000, 0.05 },
        GraphState{ 400, 0.01 },
        GraphState{ 600, 0.01 },
        GraphState{ 800, 0.01 },
        GraphState{ 1000, 0.01 },
        GraphState{ 1200, 0.01 },
        GraphState{ 1400, 0.01 },
        GraphState{ 2000, 0.001 },
    };

    const int num_requests{ 30 };
    for (auto state : states) {
        std::int64_t edges = static_cast<std::int64_t>((state.num_nodes * (state.num_nodes - 1)) * state.density);
        b->Args({state.num_nodes, edges, num_requests});
    }
}
