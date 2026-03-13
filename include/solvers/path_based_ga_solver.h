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
#include "types/graph.h"
#include "interfaces/i_genetic.h"
#include "utils/k_shortest_path_finder.h"

class PathBasedGASolver : public IGenetic, public Solver {
public:
    using PathPool = std::vector<std::vector<KShortestPathFinder::Path>>;

    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::PathBasedGAConfig& config,
                      double max_latency);
    
    double solve() override;

private:
    void build_initial_population() override;
    std::vector<double> get_population_fitness() override;
    double get_chromosome_fitness(const Chromosome& chromosome) override;
    void mutate(Chromosome& offspring) override;
    void reproduce() override;
    
    void initialise_greedy_group(std::size_t start_idx, std::size_t end_idx);
    void initialise_edge_sharing_group(std::size_t start_idx, std::size_t end_idx);
    void initialise_random_group(std::size_t start_idx, std::size_t end_idx);

    double get_path_penalty(const KShortestPathFinder::Path& path, 
                            const HFT::Request& request, 
                            int& remaining_orders, 
                            std::vector<int>& path_flow,
                            std::set<std::size_t>& used_edges);

    std::vector<Chromosome> m_cur_pop_buffer;

    PathPool m_path_pool;
    KShortestPathFinder m_ksp_finder;

    HFT::PathBasedGAConfig m_config{};
    std::mt19937 m_gen{};

    double m_best_profit{ std::numeric_limits<double>::lowest() };
    const double GREEDY_GROUP_PERC{ 0.25 };
    const double EDGE_SHARING_GROUP_PERC{ 0.5 };
};

#endif