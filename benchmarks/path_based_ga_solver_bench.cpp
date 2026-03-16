#include <cstdint>
#include <limits>

#include <benchmark/benchmark.h>

#include "bench_utils/solver_fixture.h"
#include "solvers/path_based_ga_solver.h"

BENCHMARK_DEFINE_F(SolverFixture, PathBasedGA)(benchmark::State& state) {
    double total_profit = std::numeric_limits<double>::lowest();
    double profit_sum = 0.0;

    unsigned long long best_seed = 0;
    int successes = 0;

    const int num_shortest_paths = 30;
    HFT::GAConfig solver_config {
        .population_size{ 100 },
        .generations{ 1500 },
        .mutation_rate{ 0.08 },
        .crossover_rate{ 0.8 },
    };

    for (auto _ : state) {
        unsigned long long current_seed = solver_seed_gen();
        solver_config.seed = current_seed;
        
        PathBasedGASolver solver{ 
            generator->get_graph(),
            generator->get_requests(),
            solver_config, 
            generator->get_config().max_latency,
            num_shortest_paths,
            false 
        };

        double profit = solver.solve();
        if (profit > std::numeric_limits<double>::lowest()) {
            profit_sum += profit;
            successes++;

            if (profit > total_profit) {
                total_profit = profit;
                best_seed = current_seed;
            }
        }
    }

    if (total_profit > std::numeric_limits<double>::lowest()) {
        PathBasedGASolver recorder{ 
            generator->get_graph(),
            generator->get_requests(),
            { .seed = best_seed },
            generator->get_config().max_latency, 
            num_shortest_paths, 
            true
        };

        recorder.solve();
    }

    state.counters["MeanProfit"] = (successes > 0) ? (profit_sum / successes) : 0;
    state.counters["TotalProfit"] = (successes > 0) ? total_profit : 0;
    state.counters["Reliability"] = static_cast<double>(successes) / state.iterations();
}

BENCHMARK_REGISTER_F(SolverFixture, PathBasedGA)
    ->Apply(SolverFixture::ScalingArguments)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime()
    ->Iterations(20);

BENCHMARK_MAIN();