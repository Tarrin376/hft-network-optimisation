#ifndef GENETIC_ALGORITHM_SOLVER_H
#define GENETIC_ALGORITHM_SOLVER_H

#include <vector>
#include <cstdint>
#include <random>
#include <numeric>

#include "utils/bit_utils.h"

#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"

#include "solvers/ga_solver.h"
#include "solvers/solver.h"

GASolver::GASolver(int max_order_profit, double max_latency, const HFT::GAConfig& config)
    : Solver{ max_order_profit, max_latency }
    , m_selection_evaluator{ max_order_profit, max_latency }
    , m_gen{ config.seed }
    , m_config{ config } {}

double GASolver::solve(const HFT::Graph& graph, const HFT::ExpectedRequests& requests) {
    std::vector<Chromosome> current_population{ build_initial_population(graph.get_num_edges()) };
    for (int i = 0; i < m_config.generations; ++i) {
        std::vector<Chromosome> new_population = reproduce(graph, requests, current_population);
        current_population = new_population;
    }

    return m_best_profit_achieved;
}

double GASolver::get_random_double(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(m_gen);
}

std::vector<std::size_t> GASolver::stochastic_universal_sampling(const std::vector<double>& weights) {
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
    for (std::size_t i = 0; i < offspring.size(); ++i) {
        for (int j = 0; j < 64; ++j) {
            if (get_random_double(0.0, 1.0) < m_config.mutation_rate) {
                offspring[i] ^= (1ULL << j);
            }
        }
    }
}

std::vector<GASolver::Chromosome> GASolver::build_initial_population(std::size_t num_edges) {
    std::vector<Chromosome> population(
        m_config.population_size,
        Chromosome(num_edges / 64 + 1, ~0ULL)
    );

    for (std::size_t i = 1; i < population.size(); ++i) {
        for (std::size_t j = 0; j < num_edges; ++j) {
            if (get_random_double(0.0, 1.0) < m_config.initial_bit_flip_rate) {
                population[i][j / 64] ^= (1ULL << (j % 64));
            }
        }
    }

    Chromosome random(num_edges / 64 + 1);
    for (std::size_t j = 0; j < num_edges; ++j) {
        if (get_random_double(0.0, 1.0) < 0.5) {
            random[j / 64] |= (1ULL << (j % 64));
        }
    }

    population.back() = std::move(random);
    return population;
}

std::vector<GASolver::Chromosome> GASolver::reproduce(const HFT::Graph& graph, 
                                                      const HFT::ExpectedRequests& requests, 
                                                      std::vector<Chromosome> population) {
    std::vector<double> weights(m_config.population_size, 0);
    std::vector<Chromosome> new_population{};
    
    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        double profit = m_selection_evaluator.evaluate(graph, requests, population[i]);
        m_best_profit_achieved = std::max(m_best_profit_achieved, profit);
        weights[i] = std::max(profit, 0.0);
    }

    std::vector<std::size_t> selected_parents{ stochastic_universal_sampling(weights) };

    for (std::size_t i = 0; i < m_config.population_size - 1; i += 2) {
        auto& parent1 = population[selected_parents[i]];
        auto& parent2 = population[selected_parents[i + 1]];
        
        if (get_random_double(0.0, 1.0) < m_config.crossover_rate) {
            std::uniform_int_distribution<int> point_dist(0, static_cast<int>(graph.get_num_edges()) - 1);
            int start{ point_dist(m_gen) };
            int end{ point_dist(m_gen) };
            crossover(parent1, parent2, std::min(start, end), std::max(start, end));
        }

        mutate(parent1);
        mutate(parent2);

        new_population.push_back(parent1);
        new_population.push_back(parent2);
    }

    return new_population;
}

#endif