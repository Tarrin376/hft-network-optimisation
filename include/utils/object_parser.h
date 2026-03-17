#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <memory>

#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/config.h"

namespace ObjectParser {
    std::unique_ptr<HFT::Graph> parseGraph(const std::string& nodes_file_path, const std::string& edges_file_path);
    HFT::ExpectedRequests parseExpectedRequests(const std::string& file_path);
    HFT::Config parseArgs(int argc, char* argv[]);
}

#endif