#ifndef LINK_BASED_GA_SOLVER_H
#define LINK_BASED_GA_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>

#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/ga_solver.h"
#include "types/solver.h"
#include "strategies/stochastic_universal_sampling.h"

/**
 * A Genetic Algorithm solver that optimises request routing using a 
 * link-based encoding.
 * 
 * In this approach, chromosomes represent the presence or absence of individual links 
 * (edges) in the graph.
 * 
 * Inherits from GASolver using the Curiously Recurring Template Pattern (CRTP).
 */
class LinkBasedGASolver final : public GASolver<LinkBasedGASolver, StochasticUniversalSampling> {
public:
    friend class GASolver<LinkBasedGASolver, StochasticUniversalSampling>;

    /**
     * @param graph The network topology.
     * @param requests The set of order opportunity requests to be routed.
     * @param config GA-specific hyperparameters (mutation rate, etc.).
     * @param max_latency The maximum acceptable latency considered acceptable by the firm.
     * @param record_selected_edges Whether to log final edge selections.
     */
    LinkBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency,
                      bool record_selected_edges);

private:
    bool build_initial_population();
    void mutate(HFT::Chromosome& offspring);
    void crossover(HFT::Chromosome& parent1, HFT::Chromosome& parent2);
    HFT::FitnessPair get_chromosome_fitness(const HFT::Chromosome& chromosome);

    std::uniform_int_distribution<int> m_crossover_dist;
};

#endif