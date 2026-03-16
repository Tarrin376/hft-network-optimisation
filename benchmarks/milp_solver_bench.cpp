#include <benchmark/benchmark.h>

#include "bench_utils/solver_fixture.h"
#include "solvers/milp_solver.h"

BENCHMARK_DEFINE_F(SolverFixture, MILPSolver)(benchmark::State& state) {
    double total_profit = 0.0;

    for (auto _ : state) {
        MILPSolver solver{ 
            generator->get_graph(),
            generator->get_requests(),
            { "SCIP" }, 
            generator->get_config().max_latency,
            false 
        };

        total_profit = solver.solve();
    }

    state.counters["TotalProfit"] = total_profit;
}

BENCHMARK_REGISTER_F(SolverFixture, MILPSolver)
    ->Apply(SolverFixture::ScalingArguments)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime()
    ->Iterations(1);