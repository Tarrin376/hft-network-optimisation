#include <cstdint>
#include <limits>
#include <iostream>

#include <benchmark/benchmark.h>

#include "bench_utils/solver_fixture.h"
#include "solvers/path_based_ga_solver.h"
#include "utils/logger.h"

BENCHMARK_DEFINE_F(SolverFixture, PathBasedGA)(benchmark::State& state) {
    double best_profit{ std::numeric_limits<double>::lowest() };
    double profit_sum{ 0.0 };

    const int num_shortest_paths{ 64 };
    unsigned long long best_seed{ 0ULL };
    int successes{ 0 };

    const HFT::Graph& graph{ generator->get_graph() };
    const HFT::ExpectedRequests& requests{ generator->get_requests() };

    HFT::GAConfig solver_config {
        .population_size{ 300 },
        .generations{ 500 },
        .mutation_rate{ 0.08 },
        .crossover_rate{ 0.8 },
    };

    for (auto _ : state) {
        unsigned long long current_seed = solver_seed_gen();
        solver_config.seed = current_seed;
        
        PathBasedGASolver solver{ 
            graph,
            requests,
            solver_config, 
            generator->get_config().max_latency,
            num_shortest_paths,
            false 
        };

        double profit = solver.solve();
        if (profit > std::numeric_limits<double>::lowest()) {
            profit_sum += profit;
            successes++;

            if (profit > best_profit) {
                best_profit = profit;
                best_seed = current_seed;
            }
        }
    }

    // if (best_profit > std::numeric_limits<double>::lowest()) {
    //     HFT::GAConfig recorder_config = solver_config;
    //     recorder_config.seed = best_seed;

    //     PathBasedGASolver recorder{ 
    //         graph,
    //         requests,
    //         recorder_config,
    //         generator->get_config().max_latency, 
    //         num_shortest_paths, 
    //         true
    //     };

    //     double profit = recorder.solve();
    //     auto edges = recorder.get_selected_edges();

    //     static int id{ 0 };
    //     id++;

    //     Logger::log_nodes(graph, "NODES_PB_GA" + std::to_string(id) + ".csv");
    //     Logger::log_edges(graph, "EDGES_PB_GA" + std::to_string(id) + ".csv");
    //     Logger::log_requests(requests, "REQUESTS_PB_GA" + std::to_string(id) + ".csv");
    //     Logger::log_optimal_network(edges, "ANS_PB_GA" + std::to_string(id) + ".csv");
    // }

    state.counters["MeanProfit"] = (successes > 0) ? (profit_sum / successes) : best_profit;
    state.counters["BestProfit"] = best_profit;
    state.counters["Reliability"] = static_cast<double>(successes) / state.iterations();
}

// BENCHMARK_REGISTER_F(SolverFixture, PathBasedGA)
//    ->Apply(SolverFixture::ScalingArguments)
//    ->Unit(benchmark::kMillisecond)
//    ->Iterations(10);