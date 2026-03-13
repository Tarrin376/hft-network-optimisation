#ifndef GENETIC_ALGORITHM_SOLVER_H
#define GENETIC_ALGORITHM_SOLVER_H

#include "solvers/link_based_ga_solver.h"

#include <vector>
#include <cstdint>
#include <random>
#include <numeric>
#include <cassert>

#include "utils/bit_utils.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/solver.h"

LinkBasedGASolver::LinkBasedGASolver(const HFT::Graph& graph, 
                                     const HFT::ExpectedRequests& requests, 
                                     const HFT::GAConfig& config,
                                     double max_latency)
: GASolver{ graph, requests, config, max_latency, (graph.get_num_edges() / 64) + 1 }
, m_crossover_dist(0, graph.get_num_edges() - 1) {
    warm_cache();
}

void LinkBasedGASolver::crossover(Chromosome& parent1, Chromosome& parent2) {
    if (get_random_double(0.0, 1.0) < m_config.crossover_rate) {
        return;
    }
    
    int start{ m_crossover_dist(get_gen()) };
    int end{ m_crossover_dist(get_gen()) };

    int start_idx{ std::min(start, end) };
    int end_idx{ std::max(start, end) };

    int start_block{ start_idx / 64 };
    int end_block{ end_idx / 64 };

    for (int i = start_block; i <= end_block; ++i) {
        int start_bit = (i == start_block) ? (start_idx % 64) : 0;
        int end_bit = (i == end_block) ? (end_idx % 64) : 63;

        std::uint64_t range_mask = BitUtils::mask_left(~0ULL, end_bit + 1) & BitUtils::mask_right(~0ULL, start_bit);
        std::uint64_t p1_segment = parent1[i] & range_mask;
        std::uint64_t p2_segment = parent2[i] & range_mask;

        parent1[i] = (parent1[i] & ~range_mask) | p2_segment;
        parent2[i] = (parent2[i] & ~range_mask) | p1_segment;
    }
}

void LinkBasedGASolver::mutate(Chromosome& offspring) {
    std::geometric_distribution<std::size_t> skip_dist(m_config.mutation_rate);

    std::size_t total_bits = offspring.size() * 64;
    std::size_t current_bit = skip_dist(get_gen());

    while (current_bit < total_bits) {
        std::size_t block_idx = current_bit / 64;
        std::size_t bit_idx = current_bit % 64;

        offspring[block_idx] ^= (1ULL << bit_idx);
        current_bit += (1 + skip_dist(get_gen())); 
    }
}

void LinkBasedGASolver::build_initial_population() {
    std::size_t num_edges{ m_graph.get_num_edges() };
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        for (std::size_t j = 0; j < num_edges; ++j) {
            if (i == 0 || get_random_double(0.0, 1.0) < m_config.initial_bit_flip_rate) {
                m_cur_pop_buffer[i][j / 64] |= (1ULL << (j % 64));
            }
        }
    }

    Chromosome random(num_edges / 64 + 1);
    for (std::size_t j = 0; j < num_edges; ++j) {
        if (get_random_double(0.0, 1.0) < 0.5) {
            random[j / 64] |= (1ULL << (j % 64));
        }
    }

    m_cur_pop_buffer.back() = std::move(random);
}

double LinkBasedGASolver::get_chromosome_fitness(const Chromosome& chromosome) {
    static thread_local SelectionEvaluator evaluator{ m_max_latency, m_graph, m_requests };
    return evaluator.evaluate(chromosome);
}

void LinkBasedGASolver::warm_cache() {
    // 'volatile' prevents the compiler from optimising the loop away
    volatile double dummy_sum = 0;

    for (const auto& req : m_requests) {
        dummy_sum += req.num_orders + req.server;
    }

    for (std::size_t i = 0; i < m_graph.get_num_edges(); ++i) {
        const auto& edge = m_graph.get_edge(i);
        dummy_sum += edge.latency + edge.rate_limit;
    }

    for (std::size_t i = 0; i < m_graph.get_num_nodes(); ++i) {
        const auto& node = m_graph.get_node(i);
        if (!node.outgoing_edges.empty()) {
            dummy_sum += node.outgoing_edges[0];
        }
    }
}

#endif