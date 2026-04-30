#ifndef GA_SOLVER_H
#define GA_SOLVER_H

#include <vector>
#include <cstdint>
#include <limits>
#include <random>
#include <numeric>
#include <concepts>
#include <optional>
#include <omp.h>

#include "types/solver.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "types/expected_requests.h"

namespace HFT {
    using Chromosome = std::vector<std::uint64_t>;

    struct FitnessPair {
        double fitness;
        std::vector<std::size_t> selected_edges;
    };

    template <typename SelectionStrategy>
    concept IsSelectionStrategy = requires(
        SelectionStrategy strategy, 
        const std::vector<double>& fitness, 
        std::vector<std::size_t>& next_gen_parents, 
        std::mt19937& gen) {
        { strategy.run(fitness, next_gen_parents, gen) } -> std::same_as<void>;
    };
}

template <typename T, HFT::IsSelectionStrategy SelectionStrategy>
class GASolver : public Solver {
public:
    virtual ~GASolver() = default;

    double solve() override final {
        bool success = build_initial_population();
        if (!success) {
            return m_best_profit;
        }
        
        for (int i = 0; i < m_config.generations; ++i) {
            reproduce();
        }

        return m_best_profit;
    }

protected:
    bool build_initial_population() {
        return static_cast<T*>(this)->build_initial_population();
    }

    void mutate(HFT::Chromosome& offspring) {
        return static_cast<T*>(this)->mutate(offspring);
    }

    void crossover(HFT::Chromosome& parent1, HFT::Chromosome& parent2) {
        return static_cast<T*>(this)->crossover(parent1, parent2);
    }

    HFT::FitnessPair get_chromosome_fitness(const HFT::Chromosome& chromosome) {
        return static_cast<T*>(this)->get_chromosome_fitness(chromosome);
    }

    void reproduce() {
        compute_population_fitness();
        compute_next_gen_parents();

        #pragma omp parallel
        {
            seed_thread_local(m_config.seed);

            #pragma omp for schedule(static)
            for (std::size_t i = 0; i < m_config.population_size - 1; i += 2) {
                m_next_pop_buffer[i] = m_cur_pop_buffer[m_next_gen_parents[i]];
                m_next_pop_buffer[i + 1] = m_cur_pop_buffer[m_next_gen_parents[i + 1]];

                auto& parent1 = m_next_pop_buffer[i];
                auto& parent2 = m_next_pop_buffer[i + 1];
                
                crossover(parent1, parent2);
                mutate(parent1);
                mutate(parent2);
            }
        }

        std::swap(m_cur_pop_buffer, m_next_pop_buffer);
    }

    void compute_population_fitness() {
        double gen_best_profit = std::numeric_limits<double>::lowest();
        std::vector<std::size_t> gen_best_edges;

        #pragma omp parallel
        {
            double thread_best_profit = std::numeric_limits<double>::lowest();
            std::vector<std::size_t> thread_best_edges;

            #pragma omp for schedule(dynamic)
            for (std::size_t i = 0; i < m_config.population_size; ++i) {
                HFT::FitnessPair result = get_chromosome_fitness(m_cur_pop_buffer[i]);
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

    void seed_thread_local(unsigned long long base_seed) {
        int tid = omp_get_thread_num();
        m_gen.seed(base_seed + tid);
    }

    void compute_next_gen_parents() {
        m_strategy.run(m_cur_pop_fitness, m_next_gen_parents, m_gen);
    }
    
    std::vector<std::size_t> m_next_gen_parents;
    std::vector<double> m_cur_pop_fitness;
    std::vector<HFT::Chromosome> m_cur_pop_buffer;
    std::vector<HFT::Chromosome> m_next_pop_buffer;
    
    HFT::GAConfig m_config{};
    SelectionStrategy m_strategy{};
    
    double m_best_profit{ std::numeric_limits<double>::lowest() };
    static inline thread_local std::mt19937 m_gen{};

private:
    GASolver(const HFT::Graph& graph, 
             const HFT::ExpectedRequests& requests, 
             const HFT::GAConfig& config,
             double max_latency,
             std::size_t chromosome_size,
             bool record_selected_edges)
        : Solver{ graph, requests, max_latency, record_selected_edges }
        , m_next_gen_parents(config.population_size, 0)
        , m_cur_pop_fitness(config.population_size, 0.0)
        , m_cur_pop_buffer(config.population_size, HFT::Chromosome(chromosome_size))
        , m_next_pop_buffer(config.population_size, HFT::Chromosome(chromosome_size))
        , m_config{ config } {}
    
    friend T;
};

#endif
