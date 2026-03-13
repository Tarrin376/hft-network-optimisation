#include "solvers/path_based_ga_solver.h"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <limits>
#include <set>

#include "solvers/solver.h"
#include "types/expected_requests.h"
#include "types/path_based_ga_config.h"
#include "types/graph.h"
#include "utils/k_shortest_path_finder.h"

PathBasedGASolver::PathBasedGASolver(const HFT::Graph& graph, 
                                     const HFT::ExpectedRequests& requests, 
                                     const HFT::PathBasedGAConfig& config,
                                     double max_latency)
: Solver{ graph, requests, max_latency }
, m_cur_pop_buffer(config.population_size, Chromosome(requests.size()))
, m_path_pool(requests.size())
, m_ksp_finder{ graph }
, m_config{ config }
, m_gen{ config.seed } {
    for (const auto& request : m_requests) {
        auto paths = m_ksp_finder.find_paths(request.server, request.exchange, std::min(m_config.k, 64));
        std::sort(paths.begin(), paths.end());
        m_path_pool[request.id] = std::move(paths);
    }
}

double PathBasedGASolver::solve() {
    build_initial_population();
    for (int i = 0; i < m_config.generations; ++i) {
        reproduce();
    }

    return m_best_profit;
}

void PathBasedGASolver::build_initial_population() {
    int greedy_end{ static_cast<int>(GREEDY_GROUP_PERC * m_config.population_size) };
    int arc_end{ greedy_end + static_cast<int>(EDGE_SHARING_GROUP_PERC * m_config.population_size) };

    initialise_greedy_group(0, greedy_end);
    initialise_edge_sharing_group(greedy_end, arc_end);
    initialise_random_group(arc_end, m_config.population_size);
}

std::vector<double> PathBasedGASolver::get_population_fitness() {
    std::vector<double> pop_fitness(m_config.population_size, 0.0);

    for (std::size_t i = 0; i < m_config.population_size; ++i) {
        double fitness = get_chromosome_fitness(m_cur_pop_buffer[i]);
        if (fitness > m_best_profit) {
            m_best_profit = fitness;
        }

        pop_fitness[i] = fitness;
    }

    return pop_fitness;
}

double PathBasedGASolver::get_chromosome_fitness(const Chromosome& chromosome) {
    std::set<std::size_t> global_edge_usage{};
    double total_profit{ 0.0 };

    for (std::size_t i = 0; i < m_requests.size(); ++i) {
        const auto& request = m_requests[i];
        int remaining_orders = request.num_orders;
        double request_profit = request.max_order_profit * request.num_orders;

        std::vector<int> path_flow(m_graph.get_num_edges(), 0);
        std::set<std::size_t> used_edges{};
        std::uint64_t mask = 1ULL;

        for (int j = 0; j < m_path_pool[i].size(); ++j) {
            if ((chromosome[i] & mask) > 0) {
                double path_penalty = get_path_penalty(m_path_pool[i][j], request, remaining_orders, path_flow, used_edges);
                request_profit -= path_penalty;
            }

            mask <<= 1;
        }

        if (remaining_orders > 0) {
            return std::numeric_limits<double>::lowest();
        } else {
            total_profit += request_profit;
        }

        global_edge_usage.insert(used_edges.begin(), used_edges.end());
    }

    for (auto edge_id : global_edge_usage) {
        total_profit -= m_graph.get_edge(edge_id).lease_cost;
    }

    return total_profit;
}

void PathBasedGASolver::mutate(Chromosome& offspring) {
    // mutate the chromosome
}

void PathBasedGASolver::reproduce() {
    std::vector<double> population_fitness{ get_population_fitness() };
    for (auto val : population_fitness) {
        std::cout << val << '\n';
    }
}

void PathBasedGASolver::initialise_greedy_group(std::size_t start_idx, std::size_t end_idx) {
    std::uniform_real_distribution<double> shortest_path_dist(0, 1);

    for (std::size_t i = start_idx; i < end_idx; ++i) {
        for (std::size_t j = 0; j < m_requests.size(); ++j) {
            if (m_path_pool[j].size() > 0 && shortest_path_dist(m_gen) < 0.5) {
                m_cur_pop_buffer[i][j] |= (1ULL << 0); 
            }

            if (m_path_pool[j].size() > 1 && shortest_path_dist(m_gen) < 0.5) {
                m_cur_pop_buffer[i][j] |= (1ULL << 1);
            }
        }
    }
}

void PathBasedGASolver::initialise_edge_sharing_group(std::size_t start_idx, std::size_t end_idx) {
    std::uniform_int_distribution<int> anchor_dist(0, m_requests.size() - 1);

    for (std::size_t i = start_idx; i < end_idx; ++i) {
        int anchor_req = anchor_dist(m_gen);
        const auto& backbone = m_path_pool[anchor_req][0]; 
        std::set<std::size_t> backbone_edges(backbone.edge_indices.begin(), backbone.edge_indices.end());

        for (std::size_t j = 0; j < m_requests.size(); ++j) {
            std::size_t best_path_idx = 0;
            int max_overlap = -1;

            for (std::size_t k = 0; k < m_path_pool[j].size(); ++k) {
                int current_overlap = 0;
                for (const auto& edge : m_path_pool[j][k].edge_indices) {
                    if (backbone_edges.count(edge)) {
                        current_overlap++;
                    }
                }

                if (current_overlap > max_overlap) {
                    max_overlap = current_overlap;
                    best_path_idx = k;
                }
            }

            m_cur_pop_buffer[i][j] |= (1ULL << best_path_idx);
        }
    }
}

void PathBasedGASolver::initialise_random_group(std::size_t start_idx, std::size_t end_idx) {
    std::uniform_real_distribution<double> random_dist(0, 1);

    for (std::size_t i = start_idx; i < end_idx; ++i) {
        for (std::size_t j = 0; j < m_requests.size(); ++j) {
            std::uint64_t mask = 1ULL;
            for (std::size_t k = 0; k < m_path_pool[j].size(); ++k) {
                if (random_dist(m_gen) < 0.5) {
                    m_cur_pop_buffer[i][j] |= mask;
                }

                mask <<= 1;
            }
        }
    }
}

double PathBasedGASolver::get_path_penalty(const KShortestPathFinder::Path& path, 
                                           const HFT::Request& request, 
                                           int& remaining_orders, 
                                           std::vector<int>& path_flow,
                                           std::set<std::size_t>& used_edges) {
    const double horizon = request.planning_horizon;

    int bottleneck_capacity = std::accumulate(
        path.edge_indices.begin(), 
        path.edge_indices.end(),
        std::numeric_limits<int>::max(),
        [this, horizon, &path_flow](int current_min, std::size_t edge_id) {
            int edge_capacity = m_graph.get_edge(edge_id).rate_limit * horizon - path_flow[edge_id];
            return std::min(current_min, edge_capacity);
        }
    );

    int processed_orders = std::min(bottleneck_capacity, remaining_orders);
    double path_penalty{ 0.0 };

    for (auto edge_id : path.edge_indices) {
        const auto& edge = m_graph.get_edge(edge_id);
        double penalty = processed_orders * request.max_order_profit * (edge.latency / m_max_latency);

        path_flow[edge_id] += processed_orders;
        used_edges.insert(edge_id);
        path_penalty += penalty;
    }

    remaining_orders -= processed_orders;
    return path_penalty;
}