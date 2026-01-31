#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "order_opportunity.h"
#include "object_parser.h"
#include "graph.h"

int main(int argc, char* argv[]) {
    std::string graph_file_path{};
    std::string expected_orders_path{};

    for (int i = 0; i < argc; ++i) {
        std::string arg{ argv[i] };

        if (arg == "--graph" || arg == "-g") {
            graph_file_path = argv[++i];
        } else if (arg == "--orders" || arg == "-o") {
            expected_orders_path = argv[++i];
        }
    }

    std::unique_ptr<Graph> graph{ ObjectParser::parseGraph(graph_file_path) };
    std::vector<OrderOpportunity> orderOpportunities{ 
        ObjectParser::parseOrderOpportunities(expected_orders_path) 
    };
    
    if (!graph) {
        std::cout << "Failed to parse graph.";
        return 1;
    }

    return 0;
}