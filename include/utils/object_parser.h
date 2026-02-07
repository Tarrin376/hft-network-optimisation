#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <memory>

#include "types/expected_requests.h"
#include "types/graph.h"
#include "types/config.h"

namespace ObjectParser {
    std::unique_ptr<Graph> parseGraph(const std::string& file_path);
    ExpectedRequests parseExpectedRequests(const std::string& file_path);
    Config parseArgs(int argc, char* argv[]);
}

#endif