#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <memory>

#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/config.h"

/**
 * Utility namespace for deserialising data from external sources.
 */
namespace ObjectParser {
    /**
     * Constructs a Graph object by parsing node and edge metadata from CSV files.
     * @param nodes_file_path Path to the CSV containing node definitions.
     * @param edges_file_path Path to the CSV containing directed edge definitions.
     * @return A unique_ptr to the initialised graph.
     */
    std::unique_ptr<HFT::Graph> parseGraph(const std::string& nodes_file_path, const std::string& edges_file_path);

    /**
     * Deserialises expected order requests data from a CSV file.
     * @param file_path Path to the file containing expected order requests.
     * @return A collection of ExpectedRequest objects populated with parsed data.
     */
    HFT::ExpectedRequests parseExpectedRequests(const std::string& file_path);

    /**
     * Parses command-line arguments into a Config object.
     * Handles validation of execution flags, solver selection, and hyperparameter 
     * overrides provided at runtime.
     * @param argc The count of arguments from the main entry point.
     * @param argv The array of argument strings from the main entry point.
     * @return A Config representing the validated execution parameters.
     */
    HFT::Config parseArgs(int argc, char* argv[]);
}

#endif