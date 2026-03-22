#ifndef LINK_BASED_GA_SOLVER_H
#define LINK_BASED_GA_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>
#include <limits>

#include "utils/selection_evaluator.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/ga_solver.h"
#include "types/solver.h"

class LinkBasedGASolver : public GASolver<LinkBasedGASolver> {
public:
    friend class GASolver<LinkBasedGASolver>;

    LinkBasedGASolver(const HFT::Graph& graph, 
                      const HFT::ExpectedRequests& requests, 
                      const HFT::GAConfig& config,
                      double max_latency,
                      bool record_selected_edges);

private:
    bool build_initial_population();
    FitnessPair get_chromosome_fitness(const Chromosome& chromosome);
    void mutate(Chromosome& offspring);
    void crossover(Chromosome& parent1, Chromosome& parent2);
    void warm_cache();

    std::uniform_int_distribution<int> m_crossover_dist;
};

#endif