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
    const unsigned long long seed = static_cast<unsigned long long>(state.range(3));

    HFT::GraphGenConfig config{
        .max_latency = 20.5,
        .min_rate_limit = 5,
        .max_rate_limit = 20,
        .min_lease_cost = 100,
        .max_lease_cost = 1200,
        .min_order_profit = 500,
        .max_order_profit = 7000,
        .max_planning_horizon = 10,
        .num_orders = 30,
        .num_nodes = num_nodes,
        .num_edges = num_edges,
        .num_servers = std::max(1UL, static_cast<std::size_t>(num_nodes * 0.05)),
        .num_requests = num_requests,
        .seed = seed
    };

    m_generator = std::make_unique<GraphGenerator>(config);
}

void SolverFixture::TearDown(const ::benchmark::State& _) {
    // Do Nothing
}

void SolverFixture::ScalingArguments(benchmark::internal::Benchmark* b) {
    std::vector<GraphState> states{
        GraphState{ 70, 0.30 },
        GraphState{ 250, 0.20 },
        GraphState{ 450, 0.20 },
        GraphState{ 750, 0.20 },
        GraphState{ 850, 0.20 },
        GraphState{ 950, 0.20 },
        GraphState{ 250, 0.10 },
        GraphState{ 400, 0.10 },
        GraphState{ 750, 0.05 },
        GraphState{ 1000, 0.05 },
        GraphState{ 1500, 0.05 },
        GraphState{ 2000, 0.05 },
        GraphState{ 500, 0.01 },
        GraphState{ 1000, 0.01 },
        GraphState{ 2500, 0.005 },
        GraphState{ 5000, 0.003 },
        GraphState{ 7000, 0.002 },
        GraphState{ 9000, 0.002 },
    };

    std::mt19937 setup_gen{ 34ULL };
    auto topology_seed = setup_gen();
    
    for (auto state : states) {
        std::int64_t edges = static_cast<std::int64_t>((state.num_nodes * (state.num_nodes - 1)) * state.density);
        b->Args({state.num_nodes, edges, 10, static_cast<std::int64_t>(topology_seed)});
    }
}
