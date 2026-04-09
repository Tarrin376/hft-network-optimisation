#include "solvers/link_based_ga_solver.h"

#include <vector>
#include <cstdint>
#include <iostream>

#include "utils/selection_evaluator.h"
#include "utils/bit_utils.h"
#include "utils/random_utils.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/solver.h"

LinkBasedGASolver::LinkBasedGASolver(const HFT::Graph& graph, 
                                     const HFT::ExpectedRequests& requests, 
                                     const HFT::GAConfig& config,
                                     double max_latency,
                                     bool record_selected_edges)
: GASolver{ graph, requests, config, max_latency, (graph.get_num_edges() + 63) / 64, record_selected_edges }
, m_crossover_dist(0, graph.get_num_edges() - 1) {}

bool LinkBasedGASolver::build_initial_population() {
    const std::size_t num_edges{ m_graph.get_num_edges() };
    
    #pragma omp parallel for
    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        for (std::size_t j = 0; j < num_edges; ++j) {
            if (i == 0 || RandomUtils::get_random_double(0.0, 1.0, get_gen()) < m_config.initial_bit_flip_rate) {
                m_cur_pop_buffer[i][j / 64] |= (1ULL << (j % 64));
            }
        }
    }

    HFT::Chromosome random((num_edges + 63) / 64);
    for (std::size_t i = 0; i < num_edges; ++i) {
        if (RandomUtils::get_random_double(0.0, 1.0, get_gen()) < 0.5) {
            random[i / 64] |= (1ULL << (i % 64));
        }
    }

    m_cur_pop_buffer.back() = std::move(random);
    return true;
}

void LinkBasedGASolver::mutate(HFT::Chromosome& offspring) {
    std::geometric_distribution<int> skip_dist(m_config.mutation_rate);

    const std::size_t total_bits = offspring.size() * 64;
    std::size_t current_bit = static_cast<std::size_t>(skip_dist(get_gen()));

    while (current_bit < total_bits) {
        std::size_t block_idx = current_bit / 64;
        std::size_t bit_idx = current_bit % 64;

        offspring[block_idx] ^= (1ULL << bit_idx);
        current_bit += (1 + static_cast<std::size_t>(skip_dist(get_gen()))); 
    }
}

void LinkBasedGASolver::crossover(HFT::Chromosome& parent1, HFT::Chromosome& parent2) {
    if (RandomUtils::get_random_double(0.0, 1.0, get_gen()) < m_config.crossover_rate) {
        return;
    }
    
    int first_rand_idx{ m_crossover_dist(get_gen()) };
    int second_rand_idx{ m_crossover_dist(get_gen()) };

    int start_idx{ std::min(first_rand_idx, second_rand_idx) };
    int end_idx{ std::max(first_rand_idx, second_rand_idx) };

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

void LinkBasedGASolver::reproduce() {
    compute_population_fitness();
    compute_next_gen_parents();

    #pragma omp parallel for
    for (std::size_t i = 0; i < m_config.population_size - 1; i += 2) {
        m_next_pop_buffer[i] = m_cur_pop_buffer[m_next_gen_parents[i]];
        m_next_pop_buffer[i + 1] = m_cur_pop_buffer[m_next_gen_parents[i + 1]];

        auto& parent1 = m_next_pop_buffer[i];
        auto& parent2 = m_next_pop_buffer[i + 1];
        
        crossover(parent1, parent2);
        mutate(parent1);
        mutate(parent2);
        
        HFT::FitnessPair result1 = get_chromosome_fitness(parent1);
        HFT::FitnessPair result2 = get_chromosome_fitness(parent2);

        // Repair stage to remove unused edges
        parent1 = std::move(result1.repaired_chromosome);
        parent2 = std::move(result2.repaired_chromosome);
    }

    std::swap(m_cur_pop_buffer, m_next_pop_buffer);
}

HFT::FitnessPair LinkBasedGASolver::get_chromosome_fitness(const HFT::Chromosome& chromosome) {
    SelectionEvaluator evaluator{ m_max_latency, m_graph, m_requests };
    const double fitness{ evaluator.evaluate(chromosome) };

    if (!m_record_selected_edges) {
        return { 
            .fitness = fitness, 
            .repaired_chromosome = std::move(evaluator.get_used_edges()) 
        };
    }

    std::vector<std::size_t> selected_edges{};
    for (std::size_t i = 0; i < m_graph.get_num_edges(); ++i) {
        if (chromosome[i / 64] & (1ULL << (i % 64))) {
            selected_edges.push_back(i);
        }
    }

    return { 
        .fitness = fitness,
        .repaired_chromosome = std::move(evaluator.get_used_edges()),
        .selected_edges = selected_edges
    };
}