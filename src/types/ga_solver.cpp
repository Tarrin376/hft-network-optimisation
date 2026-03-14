#include "types/ga_solver.h"

#include <vector>
#include <cstdint>
#include <limits>
#include <random>

#include "types/solver.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/expected_requests.h"

GASolver::GASolver(const HFT::Graph& graph, 
                   const HFT::ExpectedRequests& requests, 
                   const HFT::GAConfig& config,
                   double max_latency,
                   std::size_t chromosome_size)
: Solver{ graph, requests, max_latency }
, m_cur_pop_buffer(config.population_size, Chromosome(chromosome_size))
, m_next_pop_buffer(config.population_size, Chromosome(chromosome_size))
, m_config{ config } {}

double GASolver::solve() {
    build_initial_population();
    for (int i = 0; i < m_config.generations; ++i) {
        reproduce();
    }

    return m_best_profit;
}

std::vector<std::size_t> GASolver::select_next_gen_parents(const std::vector<double>& pop_fitness) {
    std::vector<std::size_t> selected_parents(m_config.population_size, 0);
    double total{ std::accumulate(pop_fitness.begin(), pop_fitness.end(), 0.0) };

    const double step{ total / static_cast<double>(m_config.population_size) };
    double cumulative{ pop_fitness[0] };

    double pointer{ get_random_double(0.0, step) };
    std::size_t idx{ 0 };

    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        while (pointer > cumulative) {
            ++idx;
            cumulative += pop_fitness[idx];
        }

        selected_parents[i] = idx;
        pointer += step;
    }

    return selected_parents;
}

std::vector<double> GASolver::get_population_fitness() {
    std::vector<double> pop_fitness(m_config.population_size, 0.0);

    #pragma omp parallel for reduction(max:m_best_profit)
    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        double fitness = get_chromosome_fitness(m_cur_pop_buffer[i]);
        if (fitness > m_best_profit) {
            m_best_profit = fitness;
        }

        pop_fitness[i] = std::max(fitness, 0.0);
    }

    return pop_fitness;
}

double GASolver::get_random_double(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(get_gen());
}

std::mt19937& GASolver::get_gen() {
    static thread_local std::mt19937 generator{ m_config.seed };
    return generator;
}

void GASolver::reproduce() {
    std::vector<double> pop_fitness{ get_population_fitness() };
    std::vector<std::size_t> selected_parents{ select_next_gen_parents(pop_fitness) };

    #pragma omp parallel for
    for (std::size_t i = 0; i < m_config.population_size - 1; i += 2) {
        m_next_pop_buffer[i] = m_cur_pop_buffer[selected_parents[i]];
        m_next_pop_buffer[i + 1] = m_cur_pop_buffer[selected_parents[i + 1]];

        auto& parent1 = m_next_pop_buffer[i];
        auto& parent2 = m_next_pop_buffer[i + 1];
        
        crossover(parent1, parent2);
        mutate(parent1);
        mutate(parent2);
    }

    std::swap(m_cur_pop_buffer, m_next_pop_buffer);
}