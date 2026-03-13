#ifndef PATH_BASED_GA_SOLVER
#define PATH_BASED_GA_SOLVER

#include <vector>
#include <cstdint>
#include <random>
#include <limits>
#include <set>

#include "types/solver.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/ga_solver.h"
#include "utils/k_shortest_path_finder.h"

class PathBasedGASolver : public GASolver {
public:
    using PathPool = std::vector<std::vector<KShortestPathFinder::Path>>;

    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency,
                      int k);

private:
    void build_initial_population() override;
    double get_chromosome_fitness(const Chromosome& chromosome) override;
    void mutate(Chromosome& offspring) override;
    void crossover(Chromosome& parent1, Chromosome& parent2) override;
    void warm_cache() override;
    
    void initialise_greedy_group(std::size_t start_idx, std::size_t end_idx);
    void initialise_edge_sharing_group(std::size_t start_idx, std::size_t end_idx);
    void initialise_random_group(std::size_t start_idx, std::size_t end_idx);

    double get_path_penalty(const KShortestPathFinder::Path& path, 
                            const HFT::Request& request, 
                            int& remaining_orders, 
                            std::vector<int>& path_flow,
                            std::set<std::size_t>& used_edges);

    PathPool m_path_pool;
    KShortestPathFinder m_ksp_finder;

    std::uniform_int_distribution<int> m_anchor_dist;

    const double GREEDY_GROUP_PERC{ 0.25 };
    const double EDGE_SHARING_GROUP_PERC{ 0.5 };
};

#endif