#include <string>
#include <memory>
#include <sstream>
#include <cstdint>

#include "types/expected_requests.h"
#include "types/graph.h"

#include "utils/object_parser.h"
#include "utils/csv_reader.h"

std::unique_ptr<HFT::Graph> ObjectParser::parseGraph(const std::string& file_path) {
    CSVReader csv_reader{ file_path };
    std::unique_ptr<HFT::Graph> graph{ nullptr };
    std::size_t id{ 0 };

    while (csv_reader.has_next()) {
        std::stringstream ss{ csv_reader.next() };
        std::string token{};

        if (!graph) {
            std::getline(ss, token, ',');
            int num_nodes{ std::stoi(token) };
            
            std::getline(ss, token, ',');
            int num_edges{ std::stoi(token) };

            graph = std::make_unique<HFT::Graph>(num_nodes, num_edges);
            continue;
        }

        HFT::Edge edge{ id };
        std::getline(ss, token, ',');
        edge.source = std::stoi(token);

        std::getline(ss, token, ',');
        edge.dest = std::stoi(token);

        std::getline(ss, token, ',');
        edge.rate_limit = std::stoi(token);

        std::getline(ss, token, ',');
        edge.latency = std::stod(token);

        std::getline(ss, token, ',');
        edge.lease_cost = std::stoull(token);

        std::getline(ss, token, ',');
        bool source_is_server = std::stoi(token);

        graph->add_edge(edge, source_is_server);
        id++;
    }

    return graph;
}

HFT::ExpectedRequests ObjectParser::parseExpectedRequests(const std::string& file_path) {
    CSVReader csv_reader{ file_path };
    HFT::ExpectedRequests requests{};

    while (csv_reader.has_next()) {
        std::stringstream ss{ csv_reader.next() };
        std::string token{};
        HFT::Request request{};

        std::getline(ss, token, ',');
        request.server = std::stoi(token);

        std::getline(ss, token, ',');
        request.exchange = std::stoi(token);

        std::getline(ss, token, ',');
        request.num_orders = std::stoi(token);

        std::getline(ss, token, ',');
        request.planning_horizon = std::stoi(token);

        requests.push_back(request);
    }

    return requests;
}

HFT::Config ObjectParser::parseArgs(int argc, char* argv[]) {
    HFT::Config config{};

    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--graph" || arg == "-g") {
            config.graph_file_path = argv[++i];
        } else if (arg == "--requests" || arg == "-o") {
            config.expected_requests_path = argv[++i];
        } else if (arg == "--algorithm" || arg == "-a") {
            config.algorithm = argv[++i];
        } else if (arg == "--maxprofit" || arg == "-p") {
            config.max_order_profit = std::stoi(argv[++i]);
        } else if (arg == "--maxlatency" || arg == "-l") {
            config.max_latency = std::stod(argv[++i]); 
        } else if (arg == "--population") {
            config.ga.population_size = std::stoi(argv[++i]);
        } else if (arg == "--generations") {
            config.ga.generations = std::stoi(argv[++i]);
        } else if (arg == "--mutation") {
            config.ga.mutation_rate = std::stod(argv[++i]);
        } else if (arg == "--crossover") {
            config.ga.crossover_rate = std::stod(argv[++i]);
        } else if (arg == "--initial-bit-flip-rate") {
            config.ga.initial_bit_flip_rate = std::stod(argv[++i]);
        } else if (arg == "--seed") {
            config.ga.seed = static_cast<unsigned>(std::stoul(argv[++i]));
        } else if (arg == "--solver-id") {
            config.milp.solver_id = argv[++i];
        }
    }

    return config;
}