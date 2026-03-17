#include "bench_utils/solver_fixture.h"

#include <cstdint>
#include <memory>
#include <random>

#include <benchmark/benchmark.h>

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
    std::vector<double> densities{ 0.01, 0.05, 0.1, 0.2, 0.5, 0.7, 0.9 };
    const int num_requests{ 1 };

    for (std::int64_t nodes = 100; nodes <= 5000; nodes *= 2) {
        for (auto density : densities) {
            std::int64_t edges = static_cast<std::int64_t>((nodes * (nodes - 1)) * density);
            b->Args({nodes, edges, num_requests});
        }
    }
}