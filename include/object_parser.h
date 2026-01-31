#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <memory>
#include <vector>

#include "order_opportunity.h"
#include "graph.h"

namespace ObjectParser {
    std::unique_ptr<Graph> parseGraph(const std::string& file_path);
    std::vector<OrderOpportunity> parseOrderOpportunities(const std::string& file_path);
}

#endif