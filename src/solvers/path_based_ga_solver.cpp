#include "solvers/path_based_ga_solver.h"

#include <vector>
#include <cstdint>
#include <limits>
#include <ranges>
#include <queue>

#include "types/solver.h"
#include "types/expected_requests.h"
#include "types/ga_config.h"
#include "types/graph.h"
#include "utils/k_shortest_path_finder.h"
#include "utils/random_utils.h"

thread_local PathBasedGASolver::Scratchpad PathBasedGASolver::m_t_scratch;

PathBasedGASolver::PathBasedGASolver(const HFT::Graph& graph, 
                                     const HFT::ExpectedRequests& requests, 
                                     const HFT::GAConfig& config,
                                     double max_latency,
                                     std::uint32_t num_shortest_paths,
                                     bool record_selected_edges,
                                     HFT::PathPoolStrategy path_pool_strategy)
: GASolver{ graph, requests, config, max_latency, requests.size(), record_selected_edges }
, m_anchor_dist(0, requests.size() - 1)
, m_path_pool(requests.size()) {
    initialise_path_pool(std::min(num_shortest_paths, 64u), path_pool_strategy);
}

bool PathBasedGASolver::build_initial_population() {
    auto empty_paths{ std::ranges::find_if(m_path_pool, [](const auto& paths) -> bool {
        return paths.empty();
    }) };

    if (empty_paths != std::ranges::end(m_path_pool)) {
        return false;
    }

    int greedy_end{ static_cast<int>(GREEDY_GROUP_PERC * m_config.population_size) };
    int edge_end{ greedy_end + static_cast<int>(EDGE_SHARING_GROUP_PERC * m_config.population_size) };

    build_greedy_group(0, edge_end);
    build_edge_sharing_group(greedy_end, edge_end);
    build_random_group(edge_end, m_config.population_size);

    return true;
}

void PathBasedGASolver::mutate(HFT::Chromosome& offspring) {
    for (std::size_t i = 0; i < offspring.size(); ++i) {
        std::uint64_t mask = 1ULL;
        for (int j = 0; j < 64; ++j) {
            if (RandomUtils::get_random_double(0.0, 1.0, get_gen()) < m_config.mutation_rate) {
                offspring[i] ^= mask;
            }

            mask <<= 1;
        }
    }
}

void PathBasedGASolver::crossover(HFT::Chromosome& parent1, HFT::Chromosome& parent2) {
    for (std::size_t i = 0; i < parent1.size(); ++i) {
        std::uint64_t mask = 1ULL;
        for (int j = 0; j < 64; ++j) {
            if (RandomUtils::get_random_double(0.0, 1.0, get_gen()) < m_config.crossover_rate) {
                std::uint64_t p1_mask = parent1[i] & mask;
                std::uint64_t p2_mask = parent2[i] & mask;

                parent1[i] = (parent1[i] & ~mask) | p2_mask;
                parent2[i] = (parent2[i] & ~mask) | p1_mask;
            }

            mask <<= 1;
        }
    }
}

HFT::FitnessPair PathBasedGASolver::get_chromosome_fitness(const HFT::Chromosome& chromosome) {
    std::size_t num_edges{ m_graph.get_num_edges() };
    m_t_scratch.ensure_capacity(num_edges);

    double total_profit{ 0.0 };
    bool is_valid_solution{ true };

    for (std::size_t i = 0; i < m_requests.size(); ++i) {
        const auto& request = m_requests[i];
        int remaining_orders = request.num_orders;

        double request_profit = request.max_order_profit * request.num_orders;
        std::uint64_t mask = 1ULL;

        for (int j = 0; j < m_path_pool[i].size(); ++j) {
            if (chromosome[i] & mask) {
                PathPenalty path_penalty = get_path_penalty(m_path_pool[i][j], request, remaining_orders);
                remaining_orders -= path_penalty.processed_orders;
                request_profit -= path_penalty.penalty;
            }

            mask <<= 1;
        }

        m_t_scratch.path_flow.assign(m_t_scratch.path_flow.size(), 0);
        total_profit += request_profit;

        if (remaining_orders > 0) {
            is_valid_solution = false;
            break;
        }
    }

    for (auto edge_id : m_t_scratch.dirty_indices) {
        total_profit -= m_graph.get_edge(edge_id).lease_cost;
        m_t_scratch.used_edges[edge_id / 64] &= ~(1ULL << (edge_id % 64));
    }
    
    if (!is_valid_solution) {
        m_t_scratch.dirty_indices.clear();
        return { .fitness = std::numeric_limits<double>::lowest() };
    }

    HFT::FitnessPair pair{ .fitness = total_profit };
    if (m_record_selected_edges) {
        pair.selected_edges = std::move(m_t_scratch.dirty_indices);
    }
    
    m_t_scratch.dirty_indices.clear();
    return pair;
}

void PathBasedGASolver::initialise_path_pool(std::uint32_t num_shortest_paths, HFT::PathPoolStrategy path_pool_strategy) {
    switch (path_pool_strategy) {
    case HFT::PathPoolStrategy::KSP_ONLY:
        initialise_ksp_only_path_pool(num_shortest_paths);
        break;
    case HFT::PathPoolStrategy::LOCAL_DIVERSIFIED:
        initialise_local_diversified_path_pool(num_shortest_paths);
        break;
    case HFT::PathPoolStrategy::GLOBAL_PENALTY:
        initialise_global_penalty_path_pool(num_shortest_paths);
        break;
    }
}

void PathBasedGASolver::initialise_local_diversified_path_pool(std::uint32_t num_shortest_paths) {
    #pragma omp parallel
    {
        HFT::Graph thread_local_graph{ m_graph };
        KShortestPathFinder ksp_finder{ thread_local_graph };

        #pragma omp for schedule(dynamic)
        for (std::size_t i = 0; i < m_requests.size(); ++i) {
            const auto& req = m_requests[i];
            ksp_finder.clear_globally_disabled_edges(); 
            m_path_pool[i] = std::move(ksp_finder.find_paths(req.server, req.exchange, num_shortest_paths));
            
            bool is_first_path{ true };
            while (m_path_pool[i].size() < 64) {
                auto best_path = std::move(ksp_finder.find_paths(req.server, req.exchange, 1));
                if (best_path.empty()) {
                    break;
                }

                for (auto edge_id : best_path[0].edge_indices) {
                    ksp_finder.globally_disable_edge(edge_id);
                }
                
                if (is_first_path) {
                    is_first_path = false;
                    continue;
                }

                m_path_pool[i].push_back(std::move(best_path[0]));
            }
        }
    }
}

void PathBasedGASolver::initialise_ksp_only_path_pool(std::uint32_t num_shortest_paths) {
    #pragma omp parallel 
    {
        KShortestPathFinder ksp_finder{ m_graph };

        #pragma omp for
        for (std::size_t i = 0; i < m_requests.size(); ++i) {
            const auto& request = m_requests[i];
            m_path_pool[i] = std::move(ksp_finder.find_paths(request.server, request.exchange, num_shortest_paths));
        }
    }
}

void PathBasedGASolver::initialise_global_penalty_path_pool(std::uint32_t num_shortest_paths) {
    initialise_ksp_only_path_pool(num_shortest_paths);

    std::vector<std::uint32_t> edge_popularities(m_graph.get_num_edges(), 0);
    HFT::Graph modified_graph{ m_graph };
    
    for (const auto& paths : m_path_pool) {
        for (const auto& path : paths) {
            for (auto edge_id : path.edge_indices) {
                edge_popularities[edge_id]++;
            }
        }
    }

    auto cmp = [&edge_popularities](std::size_t a, std::size_t b) -> bool {
        return edge_popularities[b] > edge_popularities[a];
    };

    std::priority_queue<std::size_t, std::vector<std::size_t>, decltype(cmp)> pq(cmp);
    std::size_t most_popular{ static_cast<std::size_t>(0.05 * modified_graph.get_num_edges()) };

    for (std::size_t i = 0; i < edge_popularities.size(); ++i) {
        if (pq.size() < most_popular) {
            pq.push(i);
            continue;
        }
    
        if (edge_popularities[i] > edge_popularities[pq.top()]) {
            pq.pop();
            pq.push(i);
        }
    }

    while (pq.size() > 0) {
        std::size_t edge_id = pq.top();
        const auto& edge = modified_graph.get_edge(edge_id);

        const double new_latency = edge.latency * (1 + edge_popularities[edge_id]);
        modified_graph.update_edge_latency(edge_id, new_latency);
        pq.pop();
    }

    #pragma omp parallel 
    {
        KShortestPathFinder ksp_finder{ modified_graph };

        #pragma omp for
        for (std::size_t i = 0; i < m_requests.size(); ++i) {
            const auto& request = m_requests[i];
            auto new_paths = std::move(ksp_finder.find_paths(request.server, request.exchange, 64 - num_shortest_paths));
            
            for (auto& p : new_paths) {
                m_path_pool[i].push_back(std::move(p));
            }
        }
    }
}

void PathBasedGASolver::build_greedy_group(std::size_t start_idx, std::size_t end_idx) {
    #pragma omp parallel for
    for (std::size_t i = start_idx; i < end_idx; ++i) {
        for (std::size_t j = 0; j < m_requests.size(); ++j) {
            if (m_path_pool[j].size() > 0 && RandomUtils::get_random_double(0.0, 1.0, get_gen()) < 0.5) {
                m_cur_pop_buffer[i][j] |= (1ULL << 0); 
            }

            if (m_path_pool[j].size() > 1 && RandomUtils::get_random_double(0.0, 1.0, get_gen()) < 0.5) {
                m_cur_pop_buffer[i][j] |= (1ULL << 1);
            }
        }
    }
}

void PathBasedGASolver::build_edge_sharing_group(std::size_t start_idx, std::size_t end_idx) {
    #pragma omp parallel
    {
        std::vector<std::uint32_t> edge_versions(m_graph.get_num_edges(), 0);
        std::uint32_t current_version = 0;

        #pragma omp for
        for (std::size_t i = start_idx; i < end_idx; ++i) {
            std::size_t anchor_req = static_cast<std::size_t>(m_anchor_dist(get_gen()));
            const auto& backbone = m_path_pool[anchor_req][0]; 

            current_version++; 
            for (auto e : backbone.edge_indices) {
                edge_versions[e] = current_version;
            }

            for (std::size_t j = 0; j < m_requests.size(); ++j) {
                std::size_t best_path_idx = 0;
                int max_overlap = -1;

                for (std::size_t k = 0; k < m_path_pool[j].size(); ++k) {
                    int current_overlap = 0;
                    for (const auto& edge : m_path_pool[j][k].edge_indices) {
                        if (edge_versions[edge] == current_version) {
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
}

void PathBasedGASolver::build_random_group(std::size_t start_idx, std::size_t end_idx) {
    #pragma omp parallel for
    for (std::size_t i = start_idx; i < end_idx; ++i) {
        for (std::size_t j = 0; j < m_requests.size(); ++j) {
            std::uint64_t mask = 1ULL;
            for (std::size_t k = 0; k < m_path_pool[j].size(); ++k) {
                if (RandomUtils::get_random_double(0.0, 1.0, get_gen()) < 0.5) {
                    m_cur_pop_buffer[i][j] |= mask;
                }

                mask <<= 1;
            }
        }
    }
}

PathBasedGASolver::PathPenalty PathBasedGASolver::get_path_penalty(const KShortestPathFinder::Path& path, 
                                                                   const HFT::Request& request, 
                                                                   int remaining_orders) {
    int bottleneck_capacity{ std::numeric_limits<int>::max() };

    for (auto edge_id : path.edge_indices) {
        int edge_capacity = m_graph.get_edge(edge_id).rate_limit * request.planning_horizon - m_t_scratch.path_flow[edge_id];
        bottleneck_capacity = std::min(bottleneck_capacity, edge_capacity);
    }

    int processed_orders = std::min(bottleneck_capacity, remaining_orders);
    double path_penalty{ 0.0 };

    for (auto edge_id : path.edge_indices) {
        const auto& edge = m_graph.get_edge(edge_id);
        double penalty = processed_orders * request.max_order_profit * (edge.latency / m_max_latency);

        if (!(m_t_scratch.used_edges[edge_id / 64] & (1ULL << (edge_id % 64)))) {
            m_t_scratch.dirty_indices.push_back(edge_id);
        }
        
        m_t_scratch.path_flow[edge_id] += processed_orders;
        m_t_scratch.used_edges[edge_id / 64] |= (1ULL << (edge_id % 64));
        path_penalty += penalty;
    }

    return { path_penalty, processed_orders };
}