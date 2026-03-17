#include "utils/logger.h"

#include <cstdint>
#include <vector>
#include <functional>
#include <string>

#include "types/graph.h"
#include "types/expected_requests.h"
#include "types/csv_headers.h"
#include "utils/csv_writer.h"

void Logger::log_optimal_network(const std::vector<std::reference_wrapper<const HFT::Edge>>& edges, const std::string& file_name) {
    constexpr int N{ HFT::CSVHeaders::optimal_network.size() };

    Logger::write_csv<std::reference_wrapper<const HFT::Edge>, N>(
        file_name, 
        HFT::CSVHeaders::optimal_network, 
        edges, 
        [](const std::reference_wrapper<const HFT::Edge>& edge_ref) -> const std::array<std::string, N> {
            const auto& edge = edge_ref.get();
            return {
                std::to_string(edge.source),
                std::to_string(edge.dest)
            };
        }
    );
}

void Logger::log_requests(const HFT::ExpectedRequests& requests, const std::string& file_name) {
    constexpr int N{ HFT::CSVHeaders::requests.size() };

    Logger::write_csv<HFT::Request, N>(
        file_name, 
        HFT::CSVHeaders::requests,
        requests,
        [](const HFT::Request& request) -> const std::array<std::string, N> {
            return {
                std::to_string(request.server),
                std::to_string(request.exchange),
                std::to_string(request.num_orders),
                std::to_string(request.planning_horizon),
                std::to_string(request.max_order_profit)
            };
        }
    );
}

void Logger::log_nodes(const HFT::Graph& graph, const std::string& file_name) {
    constexpr int N{ HFT::CSVHeaders::nodes.size() };
    
    std::vector<std::reference_wrapper<const HFT::Node>> nodes{};
    nodes.reserve(graph.get_num_nodes());

    for (std::size_t i = 0; i < graph.get_num_nodes(); ++i) {
        const auto& node{ graph.get_node(i) };
        nodes.push_back(std::ref(node));
    }

    Logger::write_csv<std::reference_wrapper<const HFT::Node>, N>(
        file_name, 
        HFT::CSVHeaders::nodes,
        nodes, 
        [&graph](const std::reference_wrapper<const HFT::Node>& node_ref) -> const std::array<std::string, N> {
            const auto& node = node_ref.get();
            return {
                std::to_string(node.id),
                std::to_string(node.is_server)
            };
        }
    );
}

void Logger::log_edges(const HFT::Graph& graph, const std::string& file_name) {
    constexpr int N{ HFT::CSVHeaders::edges.size() };
    std::vector<std::reference_wrapper<const HFT::Edge>> edges{};
    edges.reserve(graph.get_num_edges());

    for (std::size_t i = 0; i < graph.get_num_edges(); ++i) {
        const auto& edge{ graph.get_edge(i) };
        edges.push_back(std::ref(edge));
    }

    Logger::write_csv<std::reference_wrapper<const HFT::Edge>, N>(
        file_name, 
        HFT::CSVHeaders::edges,
        edges, 
        [&graph](const std::reference_wrapper<const HFT::Edge>& edge_ref) -> const std::array<std::string, N> {
            const auto& edge = edge_ref.get();
            return {
                std::to_string(edge.source),
                std::to_string(edge.dest),
                std::to_string(edge.rate_limit),
                std::to_string(edge.latency),
                std::to_string(edge.lease_cost)
            };
        }
    );
}