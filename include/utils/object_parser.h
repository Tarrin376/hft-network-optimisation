#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <memory>
#include <vector>

#include "types/order_opportunity.h"
#include "types/graph.h"

namespace ObjectParser {
    std::unique_ptr<Graph> parseGraph(const std::string& file_path);
    std::vector<OrderOpportunity> parseOrderOpportunities(const std::string& file_path);
}

#endif