#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include "graph.h"

struct ExpectedOrders {
    int server{};
    int exchange{};
    int num_orders{};
    int planning_horizon{};
};

class Solver {
public:
    virtual ~Solver() = default;

    virtual void run(
        const Graph& graph, 
        const std::vector<ExpectedOrders>& expectedOrders
    ) = 0;
};

#endif