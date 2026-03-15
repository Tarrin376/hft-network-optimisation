#include <iostream>
#include <string>
#include <memory>
#include <limits>

#include "utils/object_parser.h"
#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/config.h"
#include "types/solver.h"
#include "solvers/brute_force_solver.h"
#include "solvers/link_based_ga_solver.h"
#include "solvers/path_based_ga_solver.h"
#include "solvers/milp_solver.h"

std::unique_ptr<Solver> determine_solver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, const HFT::Config& config) {
    bool record_selected_edges{ config.recorded_selected_edges_path.length() > 0 };

    if (config.algorithm == "brute-force") {
        return std::make_unique<BruteForceSolver>(graph, requests, config.max_latency, record_selected_edges);
    } else if (config.algorithm == "link-based-ga") {
        return std::make_unique<LinkBasedGASolver>(graph, requests, config.ga, config.max_latency, record_selected_edges);
    } else if (config.algorithm == "path-based-ga") {
        return std::make_unique<PathBasedGASolver>(graph, requests, config.ga, config.max_latency, config.num_shortest_paths, record_selected_edges);
    } else if (config.algorithm == "milp") {
        return std::make_unique<MILPSolver>(graph, requests, config.milp, config.max_latency, record_selected_edges);
    } else {
        return nullptr;
    }
}

int main(int argc, char* argv[]) {
    HFT::Config config{ ObjectParser::parseArgs(argc, argv) };

    std::unique_ptr<HFT::Graph> graph{ ObjectParser::parseGraph(config.graph_file_path) };
    if (!graph) {
        std::cout << "Failed to parse graph. Check file name and try again.\n";
        return 1;
    }

    HFT::ExpectedRequests requests{ ObjectParser::parseExpectedRequests(config.expected_requests_path) };
    if (requests.empty()) {
        std::cout << "Failed to parse expected requests. Check file name and try again.\n";
        return 1;
    }

    std::unique_ptr<Solver> solver{ determine_solver(*graph, requests, config) };
    if (!solver) {
        std::cout << "Failed to determine solver '" << config.algorithm << "'\n";
        return 1;
    }

    double max_profit{ solver->solve() };
    if (max_profit == std::numeric_limits<double>::lowest()) {
        std::cout << "No suitable network configuration found for the given set of requests\n";
        return 0;
    }

    std::cout << "Total profit: " << max_profit << '\n';
    if (solver->get_record_selected_edges()) {
        for (const auto& edge_id : solver->get_selected_edges()) {
            const auto& edge = graph->get_edge(edge_id);
            std::cout << "From: " << edge.source << " To: " << edge.dest << '\n';
        }
    }

    return 0;
}