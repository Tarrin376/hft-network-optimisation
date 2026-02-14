#include <gtest/gtest.h>
#include <limits>

#include "solvers/brute_force_solver.h"

#include "types/expected_requests.h"
#include "types/graph.h"

class BruteForceSolverTest : public testing::Test {
protected:
    BruteForceSolverTest() : m_brute_force_solver{ 320, 40 } {}

    BruteForceSolver m_brute_force_solver;
};

TEST_F(BruteForceSolverTest, ReturnsTheGloballyOptimalProfit) {
    HFTTypes::Graph graph{ 9, 12 };
    graph.add_edge({ .id = 0, .source = 0, .dest = 5, .rate_limit = 9, .latency = 75, .lease_cost = 170 }, true);
    graph.add_edge({ .id = 1, .source = 0, .dest = 8, .rate_limit = 3, .latency = 29, .lease_cost = 120 }, true);
    graph.add_edge({ .id = 2, .source = 0, .dest = 6, .rate_limit = 5, .latency = 39, .lease_cost = 130 }, true);
    graph.add_edge({ .id = 3, .source = 6, .dest = 7, .rate_limit = 8, .latency = 41, .lease_cost = 60 }, false);
    graph.add_edge({ .id = 4, .source = 7, .dest = 3, .rate_limit = 6, .latency = 17, .lease_cost = 80 }, false);
    graph.add_edge({ .id = 5, .source = 3, .dest = 7, .rate_limit = 10, .latency = 19, .lease_cost = 230 }, false);
    graph.add_edge({ .id = 6, .source = 8, .dest = 1, .rate_limit = 2, .latency = 26, .lease_cost = 150 }, false);
    graph.add_edge({ .id = 7, .source = 5, .dest = 8, .rate_limit = 3, .latency = 50, .lease_cost = 70 }, false);
    graph.add_edge({ .id = 8, .source = 8, .dest = 4, .rate_limit = 1, .latency = 20, .lease_cost = 60 }, false);
    graph.add_edge({ .id = 9, .source = 4, .dest = 3, .rate_limit = 1, .latency = 20, .lease_cost = 60 }, false);
    graph.add_edge({ .id = 10, .source = 1, .dest = 2, .rate_limit = 2, .latency = 18, .lease_cost = 270 }, false);
    graph.add_edge({ .id = 11, .source = 2, .dest = 3, .rate_limit = 2, .latency = 15, .lease_cost = 320 }, false);

    HFTTypes::ExpectedRequests expected_requests{};
    expected_requests.push_back({ .server = 0, .exchange = 3, .num_orders = 3, .planning_horizon = 1 });

    double total_profit{ m_brute_force_solver.solve(graph, expected_requests) };
    EXPECT_EQ(total_profit, 580);
}

TEST_F(BruteForceSolverTest, ReturnsNegativeInfinityProfitWhenNoSolutionFound) {
    HFTTypes::Graph graph{ 2, 1 };
    graph.add_edge({ .id = 0, .source = 0, .dest = 1, .rate_limit = 9, .latency = 75, .lease_cost = 170 }, true);

    HFTTypes::ExpectedRequests expected_requests{};
    expected_requests.push_back({ .server = 0, .exchange = 1, .num_orders = 10, .planning_horizon = 1 });

    double total_profit{ m_brute_force_solver.solve(graph, expected_requests) };
    EXPECT_EQ(total_profit, -(std::numeric_limits<double>::infinity()));
}