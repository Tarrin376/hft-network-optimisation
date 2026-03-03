#ifndef GENETIC_ALGORITHM_SOLVER_H
#define GENETIC_ALGORITHM_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>
#include <numeric>
#include <mutex>
#include <cassert>
#include <algorithm>

#include "utils/bit_utils.h"

#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"

#include "solvers/ga_solver.h"
#include "solvers/solver.h"

GASolver::GASolver(const HFT::Graph& graph, 
                   const HFT::ExpectedRequests& requests, 
                   const HFT::GAConfig& config,
                   double max_latency)
: Solver{ graph, requests, max_latency }
, m_cur_pop_buffer(config.population_size, Chromosome(graph.get_num_edges() / 64 + 1))
, m_next_pop_buffer(config.population_size, Chromosome(graph.get_num_edges() / 64 + 1))
, m_selection_evaluator{ max_latency }
, m_config{ config } {
    warm_cache();
}

double GASolver::solve() {
    build_initial_population();
    for (int i = 0; i < m_config.generations; ++i) {
        reproduce();
    }

    return m_best_profit;
}

double GASolver::get_random_double(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(get_gen());
}

std::vector<std::size_t> GASolver::stochastic_universal_sampling(const std::vector<double>& weights) {
    assert(weights.size() == m_config.population_size && "Weights should have the same length as population size");

    std::vector<std::size_t> selected_parents(m_config.population_size, 0);
    double total{ std::accumulate(weights.begin(), weights.end(), 0.0) };

    const double step{ total / static_cast<double>(m_config.population_size) };
    double cumulative{ weights[0] };

    double pointer{ get_random_double(0.0, step) };
    std::size_t idx{ 0 };

    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        while (pointer > cumulative) {
            ++idx;
            cumulative += weights[idx];
        }

        selected_parents[i] = idx;
        pointer += step;
    }

    return selected_parents;
}

void GASolver::crossover(Chromosome& parent1, Chromosome& parent2, int start_idx, int end_idx) {
    assert(start_idx <= end_idx && "Start index must be no larger than the end index for crossover operation");

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

void GASolver::mutate(Chromosome& offspring) {
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

void GASolver::build_initial_population() {
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

void GASolver::reproduce() {
    std::vector<double> weights(m_config.population_size, 0);
    
    #pragma omp parallel for reduction(max:m_best_profit)
    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        double fitness = m_selection_evaluator.evaluate(m_graph, m_requests, m_cur_pop_buffer[i]);
        m_best_profit = std::max(m_best_profit, fitness);
        weights[i] = std::max(fitness, 0.0);
    }

    std::vector<std::size_t> selected_parents{ stochastic_universal_sampling(weights) };

    #pragma omp parallel for
    for (std::size_t i = 0; i < m_config.population_size - 1; i += 2) {
        m_next_pop_buffer[i] = m_cur_pop_buffer[selected_parents[i]];
        m_next_pop_buffer[i + 1] = m_cur_pop_buffer[selected_parents[i + 1]];

        auto& parent1 = m_next_pop_buffer[i];
        auto& parent2 = m_next_pop_buffer[i + 1];
        
        if (get_random_double(0.0, 1.0) < m_config.crossover_rate) {
            std::uniform_int_distribution<int> point_dist(0, static_cast<int>(m_graph.get_num_edges()) - 1);
            int start{ point_dist(get_gen()) };
            int end{ point_dist(get_gen()) };
            crossover(parent1, parent2, std::min(start, end), std::max(start, end));
        }

        mutate(parent1);
        mutate(parent2);
    }

    std::swap(m_cur_pop_buffer, m_next_pop_buffer);
}

void GASolver::warm_cache() {
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
            // Touch the first edge index to bring the inner vector's data page into cache
            dummy_sum += node.outgoing_edges[0];
        }
    }
}

std::mt19937& GASolver::get_gen() {
    static thread_local std::mt19937 generator{ m_config.seed };
    return generator;
}

#endif