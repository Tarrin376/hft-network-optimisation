#ifndef LOGGER_H
#define LOGGER_H

#include <cstdint>
#include <vector>
#include <functional>
#include <string>

#include "types/graph.h"
#include "types/expected_requests.h"
#include "utils/csv_writer.h"

/**
 * A namespace used as a high-level interface for the CSVWriter,
 * providing functions to export the state of the network topology,
 * expected order requests, and final optimisation results.
 */
namespace Logger {
    /**
     * A generic helper to batch-write a collection of objects to a CSV file.
     * 
     * @tparam T The type of data items in the collection.
     * @tparam N The number of columns in the output CSV.
     * @param file_name Name of the destination file.
     * @param headers Array of column header strings.
     * @param data Vector containing the objects to be logged.
     * @param formatter Function that converts an instance of T into a string array.
     */
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

    /**
     * Logs the final set of edges chosen by a solver to a CSV file.
     * @param edges A collection of references to the edges in the optimal solution.
     * @param file_name The destination CSV file name.
     */
    void log_optimal_network(const std::vector<std::reference_wrapper<const HFT::Edge>>& edges, const std::string& file_name);

    /**
     * Logs all node metadata (ID, server status) from the graph to a CSV file.
     */
    void log_nodes(const HFT::Graph& graph, const std::string& file_name);

    /**
     * Logs all link metadata (source, dest, rate-limit, latency, cost) from the graph to a CSV file.
     */
    void log_edges(const HFT::Graph& graph, const std::string& file_name);

    /**
     * Logs all order opportunity requests used during the simulation run to a CSV file.
     */
    void log_requests(const HFT::ExpectedRequests& requests, const std::string& file_name);
}

#endif