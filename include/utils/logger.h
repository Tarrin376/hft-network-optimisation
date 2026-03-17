#ifndef LOGGER_H
#define LOGGER_H

#include <cstdint>
#include <vector>
#include <functional>
#include <string>

#include "types/graph.h"
#include "types/expected_requests.h"
#include "utils/csv_writer.h"

namespace Logger {
    template <typename T, std::size_t N>
    void write_csv(const std::string& file_name,
                   const std::array<std::string, N>& headers,
                   const std::vector<T>& data,
                   const std::function<std::array<std::string, N>(const T&)> formatter) {
        CSVWriter<T, N> writer(file_name, headers, formatter);
        for (const T& item : data) {
            writer.write(item);
        }
    }

    void log_optimal_network(const std::vector<std::reference_wrapper<const HFT::Edge>>& edges, const std::string& file_name);
    void log_nodes(const HFT::Graph& graph, const std::string& file_name);
    void log_edges(const HFT::Graph& graph, const std::string& file_name);
    void log_requests(const HFT::ExpectedRequests& requests, const std::string& file_name);
}

#endif