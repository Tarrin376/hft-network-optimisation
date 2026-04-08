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
    
    std::mt19937 setup_gen(42);

    HFT::GraphGenConfig config{
        .max_latency = 20.5,
        .max_rate_limit = 4,
        .max_lease_cost = 300,
        .num_nodes = num_nodes,
        .num_edges = num_edges,
        .num_servers = num_nodes / 20,
        .num_requests = num_requests,
        .seed = setup_gen()
    };

    generator = std::make_unique<GraphGenerator>(config);
    generator->generate();
}

void SolverFixture::TearDown(const ::benchmark::State& _) {
    generator.reset();
}

void SolverFixture::ScalingArguments(benchmark::internal::Benchmark* b) {
    std::vector<GraphState> states{
        GraphState{ 100, 0.60 },
        GraphState{ 250, 0.50 },
        GraphState{ 500, 0.50 },
        GraphState{ 1000, 0.40 },
        GraphState{ 1500, 0.40 },
        GraphState{ 2000, 0.40 },
        GraphState{ 2500, 0.40 },
        GraphState{ 100, 0.15 },
        GraphState{ 200, 0.15 },
        GraphState{ 500, 0.15 },
        GraphState{ 1000, 0.10 },
        GraphState{ 2500, 0.10 },
        GraphState{ 5000, 0.05 },
        GraphState{ 7000, 0.05 },
        GraphState{ 100, 0.01 },
        GraphState{ 500, 0.01 },
        GraphState{ 1000, 0.01 },
        GraphState{ 2500, 0.01 },
        GraphState{ 5000, 0.01 },
        GraphState{ 10000, 0.01 },
        GraphState{ 50000, 0.001 },
    };

    const int num_requests{ 1 };
    for (auto state : states) {
        std::int64_t edges = static_cast<std::int64_t>((state.num_nodes * (state.num_nodes - 1)) * state.density);
        b->Args({state.num_nodes, edges, num_requests});
    }
}
