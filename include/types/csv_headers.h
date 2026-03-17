#ifndef CSV_HEADERS_H
#define CSV_HEADERS_H

#include <array>
#include <string>

namespace HFT {
    namespace CSVHeaders {
        using namespace std::string_literals;
        
        constexpr std::array optimal_network{ "From"s, "To"s };
        constexpr std::array nodes{ "NodeId"s, "IsServer"s };
        constexpr std::array edges{ "Source"s, "Dest"s, "RateLimit"s, "Latency"s, "LeaseCost"s };
        constexpr std::array requests{ "Server"s, "Exchange"s, "NumOrders"s, "PlanningHorizon"s, "MaxOrderProfit"s };
    }
}

#endif