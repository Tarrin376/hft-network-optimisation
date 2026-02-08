#include <vector>
#include <cstdint>
#include <random>

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
    std::vector<Chromosome> current_population{ build_initial_population(graph) };
    for (int i = 0; i < m_ga.generations; ++i) {
        std::vector<Chromosome> new_population{ reproduce(graph, requests, current_population) };
        current_population = new_population;
    }

    return m_max_profit;
}

std::vector<GeneticAlgorithmSolver::Chromosome> GeneticAlgorithmSolver::build_initial_population(const Graph& graph) {
    std::uniform_real_distribution<> dist(0.0, 1.0);
    std::size_t num_edges{ graph.get_num_edges() };

    std::vector<Chromosome> population(
        m_ga.population_size,
        Chromosome(num_edges, 1)
    );

    for (std::size_t i = 1; i < population.size(); ++i) {
        for (std::size_t j = 0; j < num_edges; ++j) {
            if (dist(m_gen) < m_ga.initial_bit_flip_rate) {
                population[i][j] = 0;
            }
        }
    }

    Chromosome random(num_edges);
    for (std::size_t j = 0; j < num_edges; ++j) {
        random[j] = (dist(m_gen) < 0.5);
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
        double fitness{ m_selection_evaluator.evaluate(graph, requests, population.at(i)) };
        m_max_profit = std::max(m_max_profit, fitness);
        population_fitness[i] = fitness;
    }

    std::vector<std::size_t> selected{ stochastic_universal_sampling(population_fitness) };
    std::uniform_real_distribution<> dist(0.0, 1.0);

    for (std::size_t i = 0; i < m_ga.population_size - 1; i += 2) {
        auto& parent1{ population.at(selected.at(i)) };
        auto& parent2{ population.at(selected.at(i + 1)) };
        
        if (m_ga.crossover_rate < dist(m_gen)) {
            crossover(parent1, parent2);
        }

        mutate(parent1);
        mutate(parent2);

        new_population.push_back(parent1);
        new_population.push_back(parent2);
    }

    return new_population;
}

std::vector<std::size_t> GeneticAlgorithmSolver::stochastic_universal_sampling(const std::vector<double>& population_fitness) {
    std::vector<std::size_t> selected(m_ga.population_size, 0);
    double fitness_total{ 0 };

    for (auto fitness : population_fitness) {
        fitness_total += fitness;
    }

    const double step{ fitness_total / static_cast<double>(m_ga.population_size) };
    std::uniform_real_distribution<double> dist(0.0, step);

    double pointer{ dist(m_gen) };
    double cumulative{ population_fitness.at(0) };
    std::size_t idx{ 0 };

    for (std::size_t i = 0; i < m_ga.population_size; ++i) {
        while (cumulative < pointer) {
            ++idx;
            cumulative += population_fitness.at(idx);
        }

        selected[i] = idx;
        pointer += step;
    }

    return selected;
}

void GeneticAlgorithmSolver::crossover(Chromosome& parent1, Chromosome& parent2) {
    std::uniform_int_distribution dist(0, static_cast<int>(parent1.size()));
    std::size_t start_idx{ static_cast<std::size_t>(dist(m_gen)) };
    std::size_t end_idx{ static_cast<std::size_t>(dist(m_gen)) };

    for (std::size_t i = 0; i < parent1.size(); ++i) {
        if (i >= start_idx && i <= end_idx) {
            std::uint8_t temp_gene{ parent1[i] };
            parent1[i] = parent2[i];
            parent2[i] = temp_gene;
        }
    }
}

void GeneticAlgorithmSolver::mutate(Chromosome& offspring) {
    std::uniform_real_distribution<> dist(0.0, 1.0);
    for (std::size_t i = 0; i < offspring.size(); ++i) {
        if (m_ga.mutation_rate < dist(m_gen)) {
            offspring[i] ^= 1;
        }
    }
}