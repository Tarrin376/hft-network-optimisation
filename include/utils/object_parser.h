#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <memory>
#include <vector>

#include "types/expected_requests.h"
#include "types/graph.h"

namespace ObjectParser {
    std::unique_ptr<Graph> parseGraph(const std::string& file_path);
    ExpectedRequests parseExpectedRequests(const std::string& file_path);
}

#endif