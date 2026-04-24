#ifndef STATISTICS_UTILS_H
#define STATISTICS_UTILS_H

#include "solvers/path_based_ga_solver.h"
#include "utils/k_shortest_path_finder.h"

namespace StatisticsUtils {
    double intra_pool_mean_jaccard_similarity(const PathBasedGASolver::PathPool& path_pool);
    double jaccard_similarity(const KShortestPathFinder::Path& a, const KShortestPathFinder::Path& b);
}

#endif