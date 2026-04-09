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

class LinkBasedGASolver final : public GASolver<LinkBasedGASolver, StochasticUniversalSampling> {
public:
    friend class GASolver<LinkBasedGASolver, StochasticUniversalSampling>;

    LinkBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency,
                      bool record_selected_edges);

private:
    bool build_initial_population();
    HFT::FitnessPair get_chromosome_fitness(const HFT::Chromosome& chromosome);
    void mutate(HFT::Chromosome& offspring);
    void crossover(HFT::Chromosome& parent1, HFT::Chromosome& parent2);
    void reproduce();

    std::uniform_int_distribution<int> m_crossover_dist;
};

#endif