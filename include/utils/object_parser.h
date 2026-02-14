#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <memory>

#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/config.h"

namespace ObjectParser {
    std::unique_ptr<HFTTypes::Graph> parseGraph(const std::string& file_path);
    HFTTypes::ExpectedRequests parseExpectedRequests(const std::string& file_path);
    HFTTypes::Config parseArgs(int argc, char* argv[]);
}

#endif