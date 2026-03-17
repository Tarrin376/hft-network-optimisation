#include "utils/object_parser.h"

#include <string>
#include <memory>
#include <sstream>
#include <cstdint>
#include <vector>
#include <iostream>

#include "types/expected_requests.h"
#include "types/graph.h"
#include "utils/csv_reader.h"

std::unique_ptr<HFT::Graph> ObjectParser::parseGraph(const std::string& nodes_file_path, const std::string& edges_file_path) {
    CSVReader nodes_csv_reader{ nodes_file_path };
    std::vector<HFT::Node> nodes{};
    std::size_t node_id{ 0 };

    while (nodes_csv_reader.has_next()) {
        std::stringstream ss{ nodes_csv_reader.next() };
        std::string token{};

        std::getline(ss, token, ',');
        std::size_t node_id = std::stoull(token);

        std::getline(ss, token, ',');
        bool is_server = std::stoi(token);

        HFT::Node new_node{ .id = node_id, .is_server = is_server };

        nodes.push_back(std::move(new_node));
        node_id++;
    }

    CSVReader edges_csv_reader{ edges_file_path };
    std::vector<HFT::Edge> edges{};
    std::size_t edge_id{ 0 };

    while (edges_csv_reader.has_next()) {
        std::stringstream ss{ edges_csv_reader.next() };
        std::string token{};

        HFT::Edge edge{ .id = edge_id };
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

        edges.push_back(std::move(edge));
        edge_id++;
    }

    std::unique_ptr<HFT::Graph> graph{ std::make_unique<HFT::Graph>(nodes.size(), edges.size()) };

    for (const auto& node : nodes) {
        graph->add_node(node);
    }

    for (const auto& edge : edges) {
        graph->add_edge(edge);
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

        std::getline(ss, token, ',');
        request.max_order_profit = std::stoi(token);

        requests.push_back(request);
    }

    return requests;
}

HFT::Config ObjectParser::parseArgs(int argc, char* argv[]) {
    const std::string file_prefix{ "../data_files/" };
    HFT::Config config{};

    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--nodes" || arg == "-n") {
            config.nodes_file_name = file_prefix + argv[++i];
        } else if (arg == "--edges" || arg == "-e") {
            config.edges_file_name = file_prefix + argv[++i];
        } else if (arg == "--requests" || arg == "-r") {
            config.requests_file_name = file_prefix + argv[++i];
        } else if (arg == "--record") {
            config.recorded_edges_file_name = file_prefix + argv[++i];
        } else if (arg == "--algorithm" || arg == "-a") {
            config.algorithm = argv[++i];
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
        } else if (arg == "--solver-id") {
            config.milp.solver_id = argv[++i];
        } else if (arg == "--seed") {
            config.ga.seed = std::stoull(argv[++i]);
        } else if (arg == "--num-shortest-paths") {
            config.num_shortest_paths = std::stoi(argv[++i]);
        }
    }

    return config;
}