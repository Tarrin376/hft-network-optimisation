#ifndef PATH_BASED_GA_SOLVER
#define PATH_BASED_GA_SOLVER

#include "solvers/solver.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"

class PathBasedGASolver : public Solver {
public:
    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency);
    
    double solve() override;

private:
    HFT::GAConfig m_config{};
};

#endif