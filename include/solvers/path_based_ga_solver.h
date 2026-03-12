#ifndef PATH_BASED_GA_SOLVER
#define PATH_BASED_GA_SOLVER

#include <vector>
#include <cstdint>
#include <random>
#include <limits>
#include <set>

#include "solvers/solver.h"
#include "types/expected_requests.h"
#include "types/path_based_ga_config.h"
#include "types/chromosome.h"
#include "types/graph.h"
#include "utils/k_shortest_path_finder.h"

class PathBasedGASolver : public Solver {
public:
    using PathPool = std::vector<std::vector<KShortestPathFinder::Path>>;

    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::PathBasedGAConfig& config,
                      double max_latency);
    
    double solve() override;

private:
    void build_initial_population();
    void initialise_greedy_group(std::size_t start_idx, std::size_t end_idx);
    void initialise_edge_sharing_group(std::size_t start_idx, std::size_t end_idx);
    void initialise_random_group(std::size_t start_idx, std::size_t end_idx);

    double get_chromosome_fitness(const HFT::Chromosome& chromosome);

    double get_path_penalty(const KShortestPathFinder::Path& path, 
                            const HFT::Request& request, 
                            int& remaining_orders, 
                            std::vector<int>& path_flow,
                            std::set<std::size_t>& used_edges);

    void reproduce();

    std::vector<HFT::Chromosome> m_cur_pop_buffer;

    PathPool m_path_pool;
    KShortestPathFinder m_ksp_finder;

    HFT::PathBasedGAConfig m_config{};
    std::mt19937 m_gen{};

    double m_best_profit{ std::numeric_limits<double>::lowest() };
    const double GREEDY_GROUP_PERC{ 0.25 };
    const double EDGE_SHARING_GROUP_PERC{ 0.5 };
};

#endif