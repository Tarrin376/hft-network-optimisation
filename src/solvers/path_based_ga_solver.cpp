#include "solvers/path_based_ga_solver.h"

PathBasedGASolver::PathBasedGASolver(const HFT::Graph& graph, 
                                     const HFT::ExpectedRequests& requests, 
                                     const HFT::GAConfig& config,
                                     double max_latency)
: Solver{ graph, requests, max_latency }
, m_config{ config } {}

double PathBasedGASolver::solve() {
    return 1.0;
}