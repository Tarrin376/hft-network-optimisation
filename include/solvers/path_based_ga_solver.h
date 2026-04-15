#ifndef PATH_BASED_GA_SOLVER
#define PATH_BASED_GA_SOLVER

#include <vector>
#include <cstdint>
#include <random>

#include "types/solver.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/ga_solver.h"
#include "utils/k_shortest_path_finder.h"
#include "strategies/stochastic_universal_sampling.h"

namespace HFT {
    enum class PathPoolStrategy {
        KSP_ONLY,
        GLOBAL_PENALTY,
        LOCAL_DIVERSIFIED
    };
}

class PathBasedGASolver final : public GASolver<PathBasedGASolver, StochasticUniversalSampling> {
public:
    friend class GASolver<PathBasedGASolver, StochasticUniversalSampling>;

    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency,
                      std::int32_t num_shortest_paths,
                      bool record_selected_edges,
                      HFT::PathPoolStrategy path_pool_strategy = HFT::PathPoolStrategy::KSP_ONLY);

private:
    using PathPool = std::vector<std::vector<KShortestPathFinder::Path>>;
    
    struct PathPenalty {
        double penalty{};
        int processed_orders{};
    };

    struct Scratchpad {
        std::vector<int> path_flow;
        std::vector<std::uint64_t> used_edges;
        std::vector<std::size_t> dirty_indices;

        void ensure_capacity(std::size_t num_edges) {
            if (path_flow.size() < num_edges) {
                path_flow.assign(num_edges, 0);
                used_edges.assign((num_edges + 63) / 64, 0);
                dirty_indices.reserve(num_edges / 10);
            }
        }
    };

    bool build_initial_population();
    void mutate(HFT::Chromosome& offspring);
    void crossover(HFT::Chromosome& parent1, HFT::Chromosome& parent2);
    HFT::FitnessPair get_chromosome_fitness(const HFT::Chromosome& chromosome);

    void initialise_path_pool(std::int32_t num_shortest_paths, HFT::PathPoolStrategy path_pool_strategy);
    void initialise_local_diversified_path_pool(std::int32_t num_shortest_paths);
    void initialise_ksp_only_path_pool(std::int32_t num_shortest_paths);
    void initialise_global_penalty_path_pool(std::int32_t num_shortest_paths);

    void build_greedy_group(std::size_t start_idx, std::size_t end_idx);
    void build_edge_sharing_group(std::size_t start_idx, std::size_t end_idx);
    void build_random_group(std::size_t start_idx, std::size_t end_idx);

    PathPenalty get_path_penalty(const KShortestPathFinder::Path& path, 
                                 const HFT::Request& request,
                                 int remaining_orders);

    std::uniform_int_distribution<int> m_anchor_dist;
    PathPool m_path_pool;

    const double GREEDY_GROUP_PERC{ 0.25 };
    const double EDGE_SHARING_GROUP_PERC{ 0.5 };
    static thread_local Scratchpad m_t_scratch;
};

#endif