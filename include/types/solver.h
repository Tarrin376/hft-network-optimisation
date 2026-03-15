#ifndef SOLVER_H
#define SOLVER_H

#include "types/expected_requests.h"
#include "types/graph.h"

class Solver {
public:
    virtual ~Solver() = default;

    Solver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency, bool record_selected_edges) 
    : m_graph{ graph }
    , m_requests{ requests }
    , m_max_latency{ max_latency }
    , m_record_selected_edges{ record_selected_edges } {}

    virtual double solve() = 0;

    inline const std::vector<std::size_t>& get_selected_edges() const {
        return m_selected_edges;
    }

    inline bool get_record_selected_edges() const {
        return m_record_selected_edges;
    }

protected:
    const HFT::Graph& m_graph;
    const HFT::ExpectedRequests& m_requests;
    const double m_max_latency{};

    const bool m_record_selected_edges{};
    std::vector<std::size_t> m_selected_edges;
};

#endif