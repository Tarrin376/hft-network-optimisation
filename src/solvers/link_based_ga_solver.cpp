#include "solvers/link_based_ga_solver.h"

#include <vector>
#include <cstdint>

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
, m_crossover_dist(0, graph.get_num_edges() - 1) {
    warm_cache();
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

HFT::FitnessPair LinkBasedGASolver::get_chromosome_fitness(const HFT::Chromosome& chromosome) {
    SelectionEvaluator evaluator{ m_max_latency, m_graph, m_requests };
    const double fitness{ evaluator.evaluate(chromosome) };

    if (!m_record_selected_edges) {
        return { fitness };
    }

    std::vector<std::size_t> selected_edges{};
    selected_edges.reserve(m_graph.get_num_edges());
    
    for (std::size_t i = 0; i < m_graph.get_num_edges(); ++i) {
        if (chromosome[i / 64] & (1ULL << (i % 64))) {
            selected_edges.push_back(i);
        }
    }

    return { fitness, selected_edges };
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