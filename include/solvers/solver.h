#ifndef SOLVER_H
#define SOLVER_H

#include "types/expected_requests.h"
#include "types/graph.h"

class Solver {
public:
    virtual ~Solver() = default;

    virtual double solve(
        const HFT::Graph& graph, 
        const HFT::ExpectedRequests& requests
    ) = 0;

protected:
    explicit Solver(double max_latency) : m_max_latency{ max_latency } {}

    const double m_max_latency{};
};

#endif