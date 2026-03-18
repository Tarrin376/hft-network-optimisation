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
    std::vector<std::int64_t> sparse{ 50000 };
    const int num_requests{ 1 };

    for (auto nodes : sparse) {
        std::int64_t edges = static_cast<std::int64_t>((nodes * (nodes - 1)) * 0.001);
        b->Args({nodes, edges, num_requests});
    }
}