#ifndef PATH_BASED_GA_SOLVER
#define PATH_BASED_GA_SOLVER

#include <vector>
#include <cstdint>
#include <random>
#include <deque>

#include "types/solver.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/ga_solver.h"
#include "utils/k_shortest_path_finder.h"

class PathBasedGASolver : public GASolver<PathBasedGASolver> {
public:
    friend class GASolver<PathBasedGASolver>;
    
    using PathPool = std::vector<std::deque<KShortestPathFinder::Path>>;

    struct PathPenalty {
        double penalty{};
        int processed_orders{};
    };

    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency,
                      int num_shortest_paths,
                      bool record_selected_edges);

private:
    bool build_initial_population();
    FitnessPair get_chromosome_fitness(const Chromosome& chromosome);
    void mutate(Chromosome& offspring);
    void crossover(Chromosome& parent1, Chromosome& parent2);
    void warm_cache();

    void initialise_path_pool(int num_shortest_paths);
    void build_greedy_group(std::size_t start_idx, std::size_t end_idx);
    void build_edge_sharing_group(std::size_t start_idx, std::size_t end_idx);
    void build_random_group(std::size_t start_idx, std::size_t end_idx);

    PathPenalty get_path_penalty(const KShortestPathFinder::Path& path, 
                                 const HFT::Request& request,
                                 std::vector<int>& path_flow,
                                 std::vector<std::uint64_t>& used_edges,
                                 int remaining_orders);

    PathPool m_path_pool;
    std::uniform_int_distribution<int> m_anchor_dist;

    const double GREEDY_GROUP_PERC{ 0.25 };
    const double EDGE_SHARING_GROUP_PERC{ 0.5 };
};

#endif