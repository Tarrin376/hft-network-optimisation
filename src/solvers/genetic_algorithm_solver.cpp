#include <vector>
#include <cstdint>
#include <random>
#include <iostream>

#include "utils/bit_utils.h"

#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"

#include "solvers/genetic_algorithm_solver.h"
#include "solvers/solver.h"

GeneticAlgorithmSolver::GeneticAlgorithmSolver(int max_order_profit, double max_latency, const GAConfig& ga)
    : Solver{ max_order_profit, max_latency }
    , m_gen{ ga.seed }
    , m_ga{ ga } {}

double GeneticAlgorithmSolver::solve(const Graph& graph, const ExpectedRequests& requests) {
    std::vector<Chromosome> current_population{ build_initial_population(graph.get_num_edges()) };
    for (int i = 0; i < m_ga.generations; ++i) {
        std::vector<Chromosome> new_population = reproduce(graph, requests, current_population);
        current_population = new_population;
    }

    return m_max_profit;
}

std::vector<GeneticAlgorithmSolver::Chromosome> GeneticAlgorithmSolver::build_initial_population(std::size_t num_edges) {
    std::uniform_real_distribution<> dist(0.0, 1.0);

    std::vector<Chromosome> population(
        m_ga.population_size,
        Chromosome(num_edges / 64 + 1, ~0ULL)
    );

    for (std::size_t i = 1; i < population.size(); ++i) {
        for (std::size_t j = 0; j < num_edges; ++j) {
            if (dist(m_gen) < m_ga.initial_bit_flip_rate) {
                population[i][j / 64] ^= (1ULL << (j % 64));
            }
        }
    }

    Chromosome random(num_edges / 64 + 1);
    for (std::size_t j = 0; j < num_edges; ++j) {
        if (dist(m_gen) < 0.5) {
            random[j / 64] |= (1ULL << (j % 64));
        }
    }

    population.back() = std::move(random);
    return population;
}

std::vector<GeneticAlgorithmSolver::Chromosome> GeneticAlgorithmSolver::reproduce(
const Graph& graph, 
const ExpectedRequests& requests, 
std::vector<Chromosome> population) {
    std::vector<double> population_fitness(m_ga.population_size, 0);
    std::vector<Chromosome> new_population{};
    
    for (std::size_t i = 0; i < m_ga.population_size; ++i) {
        double profit = m_selection_evaluator.evaluate(graph, requests, population.at(i));
        m_max_profit = std::max(m_max_profit, profit);
        population_fitness[i] = std::max(profit, 0.0);
    }

    std::vector<std::size_t> selected_parents{ stochastic_universal_sampling(population_fitness) };
    std::uniform_real_distribution<> dist(0.0, 1.0);

    for (std::size_t i = 0; i < m_ga.population_size - 1; i += 2) {
        auto& parent1 = population.at(selected_parents.at(i));
        auto& parent2 = population.at(selected_parents.at(i + 1));
        
        if (dist(m_gen) < m_ga.crossover_rate) {
            crossover(parent1, parent2, graph.get_num_edges());
        }

        mutate(parent1);
        mutate(parent2);

        new_population.push_back(parent1);
        new_population.push_back(parent2);
    }

    return new_population;
}

std::vector<std::size_t> GeneticAlgorithmSolver::stochastic_universal_sampling(const std::vector<double>& population_fitness) {
    std::vector<std::size_t> selected_parents(m_ga.population_size, 0);
    double fitness_total{ 0 };

    for (auto fitness : population_fitness) {
        fitness_total += fitness;
    }

    const double step{ fitness_total / static_cast<double>(m_ga.population_size) };
    std::uniform_real_distribution<double> dist(0.0, step);

    double cumulative{ population_fitness.at(0) };
    double pointer{ dist(m_gen) };
    std::size_t idx{ 0 };

    for (std::size_t i = 0; i < m_ga.population_size; ++i) {
        while (cumulative < pointer) {
            ++idx;
            cumulative += population_fitness.at(idx);
        }

        selected_parents[i] = idx;
        pointer += step;
    }

    return selected_parents;
}

void GeneticAlgorithmSolver::crossover(Chromosome& parent1, Chromosome& parent2, std::size_t num_edges) {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(num_edges) - 1);
    int first_point{ dist(m_gen) };
    int second_point{ dist(m_gen) };

    int start_idx{ std::min(first_point, second_point) };
    int end_idx{ std::max(first_point, second_point) };

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

void GeneticAlgorithmSolver::mutate(Chromosome& offspring) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (std::size_t i = 0; i < offspring.size(); ++i) {
        for (int j = 0; j < 63; ++j) {
            if (m_ga.mutation_rate < dist(m_gen)) {
                offspring[i] ^= (1ULL << j);
            }
        }
    }
}