#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "order_opportunity.h"
#include "object_parser.h"
#include "graph.h"
#include "solver.h"
#include "brute_force_solver.h"

std::unique_ptr<Solver> determineSolver(const std::string& algorithm) {
    if (algorithm == "brute_force") {
        return std::make_unique<BruteForceSolver>();
    } else {
        return nullptr;
    }
}

int main(int argc, char* argv[]) {
    std::string graph_file_path{};
    std::string expected_orders_path{};
    std::string algorithm{};

    for (int i = 0; i < argc; ++i) {
        std::string arg{ argv[i] };

        if (arg == "--graph" || arg == "-g") {
            graph_file_path = argv[++i];
        } else if (arg == "--orders" || arg == "-o") {
            expected_orders_path = argv[++i];
        } else if (arg == "--algorithm" || arg == "-a") {
            algorithm = argv[++i];
        }
    }

    std::unique_ptr<Graph> graph{ ObjectParser::parseGraph(graph_file_path) };
    if (!graph) {
        std::cout << "Failed to parse graph.";
        return 1;
    }

    std::vector<OrderOpportunity> orderOpportunities{ 
        ObjectParser::parseOrderOpportunities(expected_orders_path) 
    };

    std::unique_ptr<Solver> solver{ determineSolver(algorithm) };
    if (!solver) {
        std::cout << "Failed to determine solver.";
        return 1;
    }

    std::cout << "Total profit: " << solver->solve(*graph, orderOpportunities) << '\n';
    return 0;
}