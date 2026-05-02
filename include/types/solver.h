#ifndef SOLVER_H
#define SOLVER_H

#include <cstdint>
#include <vector>
#include <functional>

#include "types/expected_requests.h"
#include "types/graph.h"

/**
 * Abstract base class that defines the common interface for different solver implementations 
 * (GA, MILP, Brute Force).
 */
class Solver {
public:
    /**
     * @param graph The network topology.
     * @param requests The set of order opportunity requests to be routed.
     * @param max_latency The maximum acceptable latency considered acceptable by the firm.
     * @param record_selected_edges Whether to log final edge selections.
     */
    Solver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency, bool record_selected_edges);

    virtual ~Solver() = default;

    virtual double solve() = 0;

    /**
     * Retrieves the set of edges selected for the optimal network configuration.
     * This is only populated if 'record_selected_edges' was set to true.
     * @return A vector of constant references to the physical edges.
     */
    std::vector<std::reference_wrapper<const HFT::Edge>> get_selected_edges() const;

protected:
    const HFT::Graph& m_graph;
    const HFT::ExpectedRequests& m_requests;
    const double m_max_latency{};
    const bool m_record_selected_edges{};
    
    std::vector<std::size_t> m_selected_edges{};
};

#endif