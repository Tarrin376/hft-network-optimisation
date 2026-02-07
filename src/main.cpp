#include <iostream>
#include <string>
#include <memory>

#include "utils/object_parser.h"

#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/config.h"

#include "solvers/solver.h"
#include "solvers/brute_force_solver.h"
#include "solvers/genetic_algorithm_solver.h"

std::unique_ptr<Solver> determineSolver(const Config& config) {
    if (config.algorithm == "brute_force") {
        return std::make_unique<BruteForceSolver>(config.max_order_profit, config.max_latency);
    } else if (config.algorithm == "genetic") {
        return std::make_unique<GeneticAlgorithmSolver>(config.max_order_profit, config.max_latency, config.ga);
    } else {
        return nullptr;
    }
}

int main(int argc, char* argv[]) {
    Config config{ ObjectParser::parseArgs(argc, argv) };

    std::unique_ptr<Graph> graph{ ObjectParser::parseGraph(config.graph_file_path) };
    if (!graph) {
        std::cout << "Failed to parse graph. Check file name and try again.\n";
        return 1;
    }

    ExpectedRequests requests{ ObjectParser::parseExpectedRequests(config.expected_requests_path) };
    if (requests.empty()) {
        std::cout << "Failed to parse expected requests. Check file name and try again.\n";
        return 1;
    }

    std::unique_ptr<Solver> solver{ determineSolver(config) };
    if (!solver) {
        std::cout << "Failed to determine solver.";
        return 1;
    }

    double max_profit{ solver->solve(*graph, requests) };
    if (max_profit == -1) {
        std::cout << "No suitable network configuration found for the given set of requests.\n";
    } else {
        std::cout << "Total profit: " << max_profit << '\n';
    }

    return 0;
}