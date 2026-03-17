#ifndef SOLVER_H
#define SOLVER_H

#include <cstdint>
#include <vector>
#include <functional>

#include "types/expected_requests.h"
#include "types/graph.h"

class Solver {
public:
    Solver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency, bool record_selected_edges);

    virtual ~Solver() = default;

    virtual double solve() = 0;

    std::vector<std::reference_wrapper<const HFT::Edge>> get_selected_edges() const;

protected:
    const HFT::Graph& m_graph;
    const HFT::ExpectedRequests& m_requests;
    const double m_max_latency{};

    const bool m_record_selected_edges{};
    std::vector<std::size_t> m_selected_edges{};
};

#endif