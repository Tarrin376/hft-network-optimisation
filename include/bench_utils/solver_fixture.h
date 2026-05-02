#ifndef SOLVER_FIXTURE_H
#define SOLVER_FIXTURE_H

#include <random>
#include <memory>

#include <benchmark/benchmark.h>

#include "utils/graph_generator.h"

/**
 * A Google Benchmark fixture for evaluating solver performance.
 * 
 * This class handles the setup and teardown of the network environment 
 * required for benchmarking different solver implementations. It provides 
 * a consistent seed for random generators to ensure performance 
 * results are reproducible across different benchmark runs.
 */
class SolverFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state) override;
    void TearDown(const ::benchmark::State& _) override;
    static void ScalingArguments(benchmark::internal::Benchmark* b);

    static const unsigned long long solver_seed{ 123ULL };

protected:
    // Utility for random generation of graph topologies and requests.
    std::unique_ptr<GraphGenerator> m_generator;
    std::mt19937 m_solver_seed_gen{};
};

#endif