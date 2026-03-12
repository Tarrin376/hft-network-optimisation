#ifndef PATH_BASED_GA_SOLVER
#define PATH_BASED_GA_SOLVER

#include <vector>
#include <memory>

#include "solvers/solver.h"
#include "types/expected_requests.h"
#include "types/path_based_ga_config.h"
#include "types/graph.h"
#include "utils/k_shortest_path_finder.h"

class PathBasedGASolver : public Solver {
public:
    using PathPool = std::vector<std::vector<std::shared_ptr<KShortestPathFinder::Path>>>;

    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::PathBasedGAConfig& config,
                      double max_latency);
    
    double solve() override;

private:
    PathPool m_path_pool;
    KShortestPathFinder m_ksp_finder;

    HFT::PathBasedGAConfig m_config{};
};

#endif