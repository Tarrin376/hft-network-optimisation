#include "utils/statistics_utils.h"

#include <algorithm>
#include <vector>
#include <set>

#include "solvers/path_based_ga_solver.h"
#include "utils/k_shortest_path_finder.h"

double StatisticsUtils::intra_pool_mean_jaccard_similarity(const PathBasedGASolver::PathPool& path_pool) {
    double total_instance_similarity = 0.0;
    std::size_t valid_requests = 0;

    for (const auto& request_pool : path_pool) {
        if (request_pool.size() < 2) {
            continue;
        }

        double request_sum = 0.0;
        std::size_t pair_count = 0;

        // Perform an all-pairs comparison within the request pool.
        for (std::size_t i = 0; i < request_pool.size(); ++i) {
            for (std::size_t j = i + 1; j < request_pool.size(); ++j) {
                request_sum += jaccard_similarity(request_pool[i], request_pool[j]);
                pair_count++;
            }
        }

        // Normalise by the number of pairs to get the mean similarity for this request.
        total_instance_similarity += (request_sum / pair_count);
        valid_requests++;
    }

    // Return the average similarity across all requests in the path pool.
    return (valid_requests == 0) ? 0.0 : total_instance_similarity / valid_requests;
}

double StatisticsUtils::jaccard_similarity(const KShortestPathFinder::Path& p1, const KShortestPathFinder::Path& p2) {
    // Local copies are created to sort indices without modifying the original paths.
    std::vector<std::size_t> v1 = p1.edge_indices;
    std::vector<std::size_t> v2 = p2.edge_indices;

    std::ranges::sort(v1);
    std::ranges::sort(v2);
    
    std::vector<std::size_t> intersect;
    std::ranges::set_intersection(v1, v2, std::back_inserter(intersect));
    
    double union_size = v1.size() + v2.size() - intersect.size();
    return union_size == 0 ? 1.0 : intersect.size() / union_size; 
}