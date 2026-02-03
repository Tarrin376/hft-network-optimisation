#ifndef ORDER_OPPORTUNITY_H
#define ORDER_OPPORTUNITY_H

#include <cstdint>

struct OrderOpportunity {
    std::size_t server{};
    std::size_t exchange{};
    int num_orders{};
    int planning_horizon{};
};

#endif