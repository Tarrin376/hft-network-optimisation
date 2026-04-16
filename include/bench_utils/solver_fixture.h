#ifndef SOLVER_FIXTURE_H
#define SOLVER_FIXTURE_H

#include <random>
#include <memory>

#include <benchmark/benchmark.h>

#include "utils/graph_generator.h"

class SolverFixture : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state) override;
    void TearDown(const ::benchmark::State& _) override;
    static void ScalingArguments(benchmark::internal::Benchmark* b);

protected:
    std::unique_ptr<GraphGenerator> m_generator;
    std::mt19937 m_solver_seed_gen{ 123 };
};

#endif