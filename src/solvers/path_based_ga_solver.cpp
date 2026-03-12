#include "solvers/path_based_ga_solver.h"

#include "solvers/solver.h"
#include "types/expected_requests.h"
#include "types/path_based_ga_config.h"
#include "types/graph.h"

PathBasedGASolver::PathBasedGASolver(const HFT::Graph& graph, 
                                     const HFT::ExpectedRequests& requests, 
                                     const HFT::PathBasedGAConfig& config,
                                     double max_latency)
: Solver{ graph, requests, max_latency }
, m_path_pool(requests.size())
, m_ksp_finder{ graph }
, m_config{ config } {
    for (auto& request : m_requests) {
        auto paths = m_ksp_finder.find_paths(request.server, request.exchange, m_config.k);
        m_path_pool[request.id] = std::move(paths);
    }
}

double PathBasedGASolver::solve() {
    return 1.0;
}