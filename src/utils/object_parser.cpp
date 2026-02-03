#include <string>
#include <memory>
#include <sstream>
#include <cstdint>

#include "types/expected_requests.h"
#include "utils/object_parser.h"
#include "utils/csv_reader.h"
#include "types/graph.h"

std::unique_ptr<Graph> ObjectParser::parseGraph(const std::string& file_path) {
    CSVReader csv_reader{ file_path };
    std::unique_ptr<Graph> graph{ nullptr };
    std::size_t id{ 0 };

    while (csv_reader.has_next()) {
        std::stringstream ss{ csv_reader.next() };
        std::string token{};

        if (!graph) {
            std::getline(ss, token, ',');
            int num_nodes{ std::stoi(token) };
            
            std::getline(ss, token, ',');
            int num_edges{ std::stoi(token) };

            graph = std::make_unique<Graph>(num_nodes, num_edges);
            continue;
        }

        Edge edge{ id };
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

ExpectedRequests ObjectParser::parseExpectedRequests(const std::string& file_path) {
    CSVReader csv_reader{ file_path };
    ExpectedRequests requests{};

    while (csv_reader.has_next()) {
        std::stringstream ss{ csv_reader.next() };
        std::string token{};
        Request request{};

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