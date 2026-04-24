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

        for (std::size_t i = 0; i < request_pool.size(); ++i) {
            for (std::size_t j = i + 1; j < request_pool.size(); ++j) {
                request_sum += jaccard_similarity(request_pool[i], request_pool[j]);
                pair_count++;
            }
        }

        total_instance_similarity += (request_sum / pair_count);
        valid_requests++;
    }

    return (valid_requests == 0) ? 0.0 : total_instance_similarity / valid_requests;
}

double StatisticsUtils::jaccard_similarity(const KShortestPathFinder::Path& p1, const KShortestPathFinder::Path& p2) {
    std::vector<std::size_t> v1 = p1.edge_indices;
    std::vector<std::size_t> v2 = p2.edge_indices;

    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    
    std::vector<std::size_t> intersect;
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(intersect));

    double union_size = v1.size() + v2.size() - intersect.size();
    return union_size == 0 ? 1.0 : intersect.size() / union_size; 
}