#include <limits>
#include <cstdint>

#include <gtest/gtest.h>

#include "solvers/brute_force_solver.h"
#include "types/expected_requests.h"
#include "types/graph.h"

TEST(BruteForceSolverTest, ReturnsTheGloballyOptimalProfit) {
    HFT::Graph graph{ 9, 12 };
    graph.add_node({ 0, true });

    for (std::size_t i = 1; i <= 8; ++i) {
        graph.add_node({ i, false });
    }

    graph.add_edge({ .id = 0, .source = 0, .dest = 5, .rate_limit = 9, .latency = 75, .lease_cost = 170 });
    graph.add_edge({ .id = 1, .source = 0, .dest = 8, .rate_limit = 3, .latency = 29, .lease_cost = 40 });
    graph.add_edge({ .id = 2, .source = 0, .dest = 6, .rate_limit = 5, .latency = 39, .lease_cost = 130 });
    graph.add_edge({ .id = 3, .source = 6, .dest = 7, .rate_limit = 8, .latency = 41, .lease_cost = 60 });
    graph.add_edge({ .id = 4, .source = 7, .dest = 3, .rate_limit = 6, .latency = 17, .lease_cost = 20 });
    graph.add_edge({ .id = 5, .source = 3, .dest = 7, .rate_limit = 10, .latency = 19, .lease_cost = 15 });
    graph.add_edge({ .id = 6, .source = 8, .dest = 1, .rate_limit = 2, .latency = 26, .lease_cost = 15 });
    graph.add_edge({ .id = 7, .source = 5, .dest = 8, .rate_limit = 3, .latency = 50, .lease_cost = 70 });
    graph.add_edge({ .id = 8, .source = 8, .dest = 4, .rate_limit = 1, .latency = 20, .lease_cost = 30 });
    graph.add_edge({ .id = 9, .source = 4, .dest = 3, .rate_limit = 1, .latency = 20, .lease_cost = 30 });
    graph.add_edge({ .id = 10, .source = 1, .dest = 2, .rate_limit = 2, .latency = 18, .lease_cost = 27 });
    graph.add_edge({ .id = 11, .source = 2, .dest = 3, .rate_limit = 2, .latency = 15, .lease_cost = 32 });

    HFT::ExpectedRequests requests{};
    requests.push_back({ 
        .server = 0, 
        .exchange = 3, 
        .num_orders = 3, 
        .planning_horizon = 1, 
        .max_order_profit = 360 
    });

    BruteForceSolver brute_force_solver{ graph, requests, 100, false };

    double total_profit{ brute_force_solver.solve() };
    EXPECT_FLOAT_EQ(total_profit, 24);
}

TEST(BruteForceSolverTest, ReturnsNegativeInfinityProfitWhenNoSolutionFound) {
    HFT::Graph graph{ 2, 1 };
    graph.add_node({ 0, true });
    graph.add_node({ 1, false });
    graph.add_edge({ .id = 0, .source = 0, .dest = 1, .rate_limit = 9, .latency = 75, .lease_cost = 170 });

    HFT::ExpectedRequests requests{};
    requests.push_back({ 
        .server = 0, 
        .exchange = 1, 
        .num_orders = 10, 
        .planning_horizon = 1, 
        .max_order_profit = 360 
    });

    BruteForceSolver brute_force_solver{ graph, requests, 100, false };

    double total_profit{ brute_force_solver.solve() };
    EXPECT_FLOAT_EQ(total_profit, std::numeric_limits<double>::lowest());
}