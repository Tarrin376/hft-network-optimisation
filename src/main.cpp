#include <iostream>
#include <string>
#include <memory>

#include "types/expected_requests.h"
#include "utils/object_parser.h"
#include "types/graph.h"
#include "solvers/solver.h"
#include "solvers/brute_force_solver.h"

std::unique_ptr<Solver> determineSolver(const std::string& algorithm, int max_order_profit, double max_latency) {
    if (algorithm == "brute_force") {
        return std::make_unique<BruteForceSolver>(max_order_profit, max_latency);
    } else {
        return nullptr;
    }
}

int main(int argc, char* argv[]) {
    std::string graph_file_path{};
    std::string expected_requests_path{};
    std::string algorithm{};

    int max_order_profit{};
    double max_latency{};

    for (int i = 0; i < argc; ++i) {
        std::string arg{ argv[i] };

        if (arg == "--graph" || arg == "-g") {
            graph_file_path = argv[++i];
        } else if (arg == "--requests" || arg == "-o") {
            expected_requests_path = argv[++i];
        } else if (arg == "--algorithm" || arg == "-a") {
            algorithm = argv[++i];
        } else if (arg == "--maxprofit" || arg == "-p") {
            max_order_profit = std::stoi(argv[++i]);
        } else if (arg == "--maxlatency" || arg == "-l") {
            max_latency = std::stod(argv[++i]); 
        }
    }

    std::unique_ptr<Graph> graph{ ObjectParser::parseGraph(graph_file_path) };
    if (!graph) {
        std::cout << "Failed to parse graph. Check file name and try again.\n";
        return 1;
    }

    ExpectedRequests requests{ ObjectParser::parseExpectedRequests(expected_requests_path) };
    if (requests.empty()) {
        std::cout << "Failed to parse expected requests. Check file name and try again.\n";
        return 1;
    }

    std::unique_ptr<Solver> solver{ determineSolver(algorithm, max_order_profit, max_latency) };
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