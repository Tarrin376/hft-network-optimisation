#include <benchmark/benchmark.h>

#include "bench_utils/solver_fixture.h"
#include "utils/logger.h"
#include "solvers/milp_solver.h"

BENCHMARK_DEFINE_F(SolverFixture, MILPSolver)(benchmark::State& state) {
    double best_profit{ std::numeric_limits<double>::lowest() };

    const HFT::Graph& graph{ generator->get_graph() };
    const HFT::ExpectedRequests& requests{ generator->get_requests() };

    for (auto _ : state) {
        MILPSolver solver{ 
            graph,
            requests,
            { "SCIP" }, 
            generator->get_config().max_latency,
            false 
        };

        best_profit = solver.solve();
    }

    if (best_profit > std::numeric_limits<double>::lowest()) {
        MILPSolver recorder{ 
            graph,
            requests,
            { "SCIP" }, 
            generator->get_config().max_latency,
            true 
        };

        recorder.solve();
        auto edges = recorder.get_selected_edges();

        static int id{ 0 };
        id++;

        Logger::log_nodes(graph, "NODES_MILP_" + std::to_string(id) + ".csv");
        Logger::log_edges(graph, "EDGES_MILP_" + std::to_string(id) + ".csv");
        Logger::log_requests(requests, "REQUESTS_MILP_" + std::to_string(id) + ".csv");
        Logger::log_optimal_network(edges, "ANS_MILP_" + std::to_string(id) + ".csv");
    }

    state.counters["BestProfit"] = best_profit;
}

// BENCHMARK_REGISTER_F(SolverFixture, MILPSolver)
//     ->Apply(SolverFixture::ScalingArguments)
//     ->Unit(benchmark::kMillisecond)
//     ->Iterations(1);
