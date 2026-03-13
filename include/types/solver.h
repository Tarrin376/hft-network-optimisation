#ifndef SOLVER_H
#define SOLVER_H

#include "types/expected_requests.h"
#include "types/graph.h"

class Solver {
public:
    virtual ~Solver() = default;

    Solver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency) 
    : m_graph{ graph }
    , m_requests{ requests }
    , m_max_latency{ max_latency } {}

    virtual double solve() = 0;

protected:
    const HFT::Graph& m_graph;
    const HFT::ExpectedRequests& m_requests;
    const double m_max_latency{};
};

#endif