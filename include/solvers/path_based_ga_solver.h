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

/**
 * A Genetic Algorithm solver that optimises request routing using a pre-computed 
 * pool of candidate paths.
 * 
 * This class implements a path-based approach where chromosomes represent 
 * indices into a path pool rather than graph edges. It supports multiple 
 * strategies for path pool initialisation and population diversification.
 * 
 * Inherits from GASolver using the Curiously Recurring Template Pattern (CRTP).
 */
class PathBasedGASolver final : public GASolver<PathBasedGASolver, StochasticUniversalSampling> {
public:
    friend class GASolver<PathBasedGASolver, StochasticUniversalSampling>;

    // A 2D collection where PathPool[request_index][path_index] returns a Path.
    using PathPool = std::vector<std::vector<KShortestPathFinder::Path>>;

    /**
     * @param graph The network topology.
     * @param requests The set of order opportunity requests to be routed.
     * @param config GA-specific hyperparameters (mutation rate, etc.).
     * @param max_latency The maximum acceptable latency considered acceptable by the firm.
     * @param num_shortest_paths Number of candidate paths (k) per request.
     * @param record_selected_edges Whether to log final edge selections.
     * @param path_pool_strategy Strategy used to populate the path pool.
     */
    PathBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency,
                      std::int32_t num_shortest_paths,
                      bool record_selected_edges,
                      HFT::PathPoolStrategy path_pool_strategy = HFT::PathPoolStrategy::KSP_ONLY);

    const PathPool& get_path_pool();

private:
    // Tracks penalties and order throughput for a specific path candidate.
    struct PathPenalty {
        double penalty{};
        int processed_orders{};
    };

    /**
     * Thread-local optimisation struct to avoid repeated allocations during 
     * fitness evaluations across multiple threads.
     */
    struct Scratchpad {
        std::vector<int> path_flow;
        std::vector<std::uint64_t> used_edges;
        std::vector<std::size_t> dirty_indices;

        // Resizes buffers if the current graph size exceeds allocated capacity.
        void ensure_capacity(std::size_t num_edges) {
            if (path_flow.size() < num_edges) {
                path_flow.assign(num_edges, 0);
                used_edges.assign((num_edges + 63) / 64, 0);
                dirty_indices.reserve(num_edges / 10);
            }
        }
    };

    // GA Lifecycle and Operators
    bool build_initial_population();
    void mutate(HFT::Chromosome& offspring);
    void crossover(HFT::Chromosome& parent1, HFT::Chromosome& parent2);
    HFT::FitnessPair get_chromosome_fitness(const HFT::Chromosome& chromosome);

    // Path Pool Initialisation Strategies
    void initialise_path_pool(std::int32_t num_shortest_paths, HFT::PathPoolStrategy path_pool_strategy);
    void initialise_local_diversified_path_pool(std::int32_t num_shortest_paths);
    void initialise_ksp_only_path_pool(std::int32_t num_shortest_paths);
    void initialise_global_penalty_path_pool(std::int32_t num_shortest_paths);

    // Population Construction
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