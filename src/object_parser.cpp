#include <string>
#include <memory>
#include <sstream>

#include "order_opportunity.h"
#include "object_parser.h"
#include "csv_reader.h"
#include "graph.h"

std::unique_ptr<Graph> ObjectParser::parseGraph(const std::string& file_path) {
    CSVReader csv_reader{ file_path };
    Graph graph{};
    int id{ 0 };

    while (csv_reader.has_next()) {
        std::stringstream ss{ csv_reader.next() };
        std::string token{};
        Edge edge{ ++id };
        
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

        graph.add_edge(edge, source_is_server);
    }

    return std::make_unique<Graph>(graph);
}

std::vector<OrderOpportunity> ObjectParser::parseOrderOpportunities(const std::string& file_path) {
    CSVReader csv_reader{ file_path };
    std::vector<OrderOpportunity> orderOpportunities{};

    while (csv_reader.has_next()) {
        std::stringstream ss{ csv_reader.next() };
        std::string token{};
        OrderOpportunity orderOpportunity{};

        std::getline(ss, token, ',');
        orderOpportunity.server = std::stoi(token);

        std::getline(ss, token, ',');
        orderOpportunity.exchange = std::stoi(token);

        std::getline(ss, token, ',');
        orderOpportunity.num_orders = std::stoi(token);

        std::getline(ss, token, ',');
        orderOpportunity.planning_horizon = std::stoi(token);

        orderOpportunities.push_back(orderOpportunity);
    }

    return orderOpportunities;
}