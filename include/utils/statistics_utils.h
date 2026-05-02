#ifndef STATISTICS_UTILS_H
#define STATISTICS_UTILS_H

#include "solvers/path_based_ga_solver.h"
#include "utils/k_shortest_path_finder.h"

/**
 * Utility functions for calculating statistical metrics.
 */
namespace StatisticsUtils {
    /**
     * Calculates the average Jaccard Similarity across all pairs in a path pool.
     * 
     * This metric is used to measure diversity within the path pools of various
     * diversification strategies such as HAD (Heuristic Arc-Disjoint), SLO (Static Latency-Optimised),
     * and GCA (Global Congestion-Aware). The higher the mean similarity, the less diverse the path pool is.
     * @param path_pool The collection of paths being used by the Path-Based GA solver.
     * @return The mean Jaccard Similarity (0.0 to 1.0).
     */
    double intra_pool_mean_jaccard_similarity(const PathBasedGASolver::PathPool& path_pool);

    /**
     * Computes the Jaccard Similarity between two specific paths.
     * @param a The first path to compare.
     * @param b The second path to compare.
     * @return A value where 1.0 indicates identical paths and 0.0 indicates completely disjoint edge sets.
     */
    double jaccard_similarity(const KShortestPathFinder::Path& a, const KShortestPathFinder::Path& b);
}

#endif