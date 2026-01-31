#ifndef ORDER_OPPORTUNITY_H
#define ORDER_OPPORTUNITY_H

struct OrderOpportunity {
    int server{};
    int exchange{};
    int num_orders{};
    int planning_horizon{};
};

#endif