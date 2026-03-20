#include "types/ga_solver.h"

#include <vector>
#include <cstdint>
#include <limits>
#include <random>
#include <iostream>

#include "types/solver.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/expected_requests.h"

GASolver::GASolver(const HFT::Graph& graph, 
                   const HFT::ExpectedRequests& requests, 
                   const HFT::GAConfig& config,
                   double max_latency,
                   std::size_t chromosome_size,
                   bool record_selected_edges)
: Solver{ graph, requests, max_latency, record_selected_edges }
, m_next_gen_parents(config.population_size, 0)
, m_cur_pop_fitness(config.population_size, 0.0)
, m_cur_pop_buffer(config.population_size, Chromosome(chromosome_size))
, m_next_pop_buffer(config.population_size, Chromosome(chromosome_size))
, m_config{ config } {}

double GASolver::solve() {
    bool success = build_initial_population();
    if (!success) {
        return m_best_profit;
    }
    
    for (int i = 0; i < m_config.generations; ++i) {
        reproduce();
    }

    return m_best_profit;
}

void GASolver::compute_next_gen_parents() {
    double total{ std::accumulate(m_cur_pop_fitness.begin(), m_cur_pop_fitness.end(), 0.0) };
    const double step{ total / static_cast<double>(m_config.population_size) };
    double cumulative{ m_cur_pop_fitness[0] };

    double pointer{ get_random_double(0.0, step) };
    std::size_t idx{ 0 };

    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        while (pointer > cumulative && idx < m_config.population_size - 1) {
            ++idx;
            cumulative += m_cur_pop_fitness[idx];
        }

        m_next_gen_parents[i] = idx;
        pointer += step;
    }
}

void GASolver::compute_population_fitness() {
    double gen_best_profit = std::numeric_limits<double>::lowest();
    std::vector<std::size_t> gen_best_edges;

    #pragma omp parallel
    {
        double thread_best_profit = std::numeric_limits<double>::lowest();
        std::vector<std::size_t> thread_best_edges;

        #pragma omp for
        for (std::size_t i = 0; i < m_config.population_size; ++i) {
            FitnessPair result = get_chromosome_fitness(m_cur_pop_buffer[i]);
            m_cur_pop_fitness[i] = std::max(result.fitness, 0.0);

            if (result.fitness > thread_best_profit) {
                thread_best_profit = result.fitness;
                if (m_record_selected_edges) {
                    thread_best_edges = std::move(result.selected_edges);
                }
            }
        }

        #pragma omp critical
        {
            if (thread_best_profit > gen_best_profit) {
                gen_best_profit = thread_best_profit;
                if (m_record_selected_edges) {
                    gen_best_edges = std::move(thread_best_edges);
                }
            }
        }
    }

    if (gen_best_profit > m_best_profit) {
        m_best_profit = gen_best_profit;
        if (m_record_selected_edges) {
            m_selected_edges = std::move(gen_best_edges);
        }
    }
}

double GASolver::get_random_double(double min, double max) {
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return min + (max - min) * dist(get_gen());
}

std::mt19937& GASolver::get_gen() {
    static thread_local std::mt19937 generator{ m_config.seed };
    return generator;
}

void GASolver::reproduce() {
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
    }

    std::swap(m_cur_pop_buffer, m_next_pop_buffer);
}