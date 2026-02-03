#ifndef ORDER_OPPORTUNITY_H
#define ORDER_OPPORTUNITY_H

#include <cstdint>
#include <vector>

struct Request {
    std::size_t server{};
    std::size_t exchange{};
    int num_orders{};
    int planning_horizon{};
};

using ExpectedRequests = std::vector<Request>;

#endif