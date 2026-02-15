#include <cstdlib>
#include <memory>
#include <cstdint>
#include <vector>
#include <string>

#include "absl/base/log_severity.h"
#include "absl/log/globals.h"
#include "absl/log/log.h"
#include "ortools/base/init_google.h"
#include "ortools/init/init.h"
#include "ortools/linear_solver/linear_solver.h"

#include "types/expected_requests.h"
#include "types/milp_config.h"
#include "types/graph.h"

#include "solvers/milp_solver.h"
#include "solvers/solver.h"

MILPSolver::MILPSolver(int max_order_profit, double max_latency, const HFT::MILPConfig& config) 
: Solver{ max_order_profit, max_latency }
, m_config{ config } {
    set_solver(config.solver_id);
}

void MILPSolver::set_solver(const std::string& solver_id) {
    std::unique_ptr<or_tools::MPSolver> new_solver(or_tools::MPSolver::CreateSolver(solver_id));
    if (!new_solver) {
        LOG(WARNING) << "Could not create solver " << solver_id;
        return;
    }

    m_solver = std::move(new_solver);
    m_config.solver_id = solver_id;
}

std::vector<or_tools::MPVariable*> MILPSolver::build_edge_variables(const HFT::Graph& graph, 
                                                                    const HFT::ExpectedRequests& requests) {
    std::vector<or_tools::MPVariable*> edge_vars{};
    for (std::size_t i = 0; i < graph.get_num_edges(); ++i) {
        std::string edge_label = "y" + std::to_string(i);
        edge_vars.emplace_back(m_solver->MakeBoolVar(edge_label));
    }

    return edge_vars;
}

std::vector<or_tools::MPVariable*> MILPSolver::build_flow_variables(const HFT::Graph& graph, 
                                                                    const HFT::ExpectedRequests& requests) {
    std::vector<or_tools::MPVariable*> flow_vars{};
    for (size_t i = 0; i < requests.size(); ++i) {
        for (size_t a = 0; a < graph.get_num_edges(); ++a) {
            std::string flow_label = "f" + std::to_string(i) + "," + std::to_string(a);
            flow_vars.emplace_back(m_solver->MakeNumVar(0.0, requests[i].num_orders, flow_label));
        }
    }

    return flow_vars;
}

void MILPSolver::apply_flow_conservation_constraints(const HFT::Graph& graph, 
                                                     const HFT::ExpectedRequests& requests,
                                                     const std::vector<or_tools::MPVariable*> flow_vars) {
    for (size_t i = 0; i < requests.size(); ++i) {
        const auto& req = requests[i];
        
        for (size_t v = 0; v < graph.get_num_nodes(); ++v) {
            // Set the RHS (Right Hand Side) based on if v is a server, exchange, or an intermediate node
            double rhs = v == req.server ? req.num_orders : (v == req.exchange ? -req.num_orders : 0);
            
            // Create the constraint: sum(out) - sum(in) = rhs
            or_tools::MPConstraint* const flow_con = m_solver->MakeRowConstraint(rhs, rhs);
            const auto& node = graph.get_node(v);

            // Outgoing edges: sum(f_{i,a})
            for (const auto& edge_idx : node.outgoing_edges) {
                flow_con->SetCoefficient(flow_vars[i * graph.get_num_edges() + edge_idx], 1.0);
            }

            // Incoming edges: -sum(f_{i,a})
            for (const auto& edge_idx : node.incoming_edges) {
                flow_con->SetCoefficient(flow_vars[i * graph.get_num_edges() + edge_idx], -1.0);
            }
        }
    }
}

void MILPSolver::apply_capacity_constraints(const HFT::Graph& graph, 
                                            const HFT::ExpectedRequests& requests,
                                            const std::vector<or_tools::MPVariable*> flow_vars,
                                            const std::vector<or_tools::MPVariable*> edge_vars) {
    for (size_t i = 0; i < requests.size(); ++i) {
        for (size_t a = 0; a < graph.get_num_edges(); ++a) {
            or_tools::MPConstraint* const cap_con = m_solver->MakeRowConstraint(-or_tools::MPSolver::infinity(), 0.0);
            double capacity_coeff = graph.get_edge(a).rate_limit * requests[i].planning_horizon;
            
            cap_con->SetCoefficient(flow_vars[i * graph.get_num_edges() + a], 1.0);
            cap_con->SetCoefficient(edge_vars[a], -capacity_coeff);
        }
    }
}

or_tools::MPObjective* const MILPSolver::build_objective_function(const HFT::Graph& graph, 
                                                                  const HFT::ExpectedRequests& requests,
                                                                  const std::vector<or_tools::MPVariable*> flow_vars,
                                                                  const std::vector<or_tools::MPVariable*> edge_vars) {
    or_tools::MPObjective* const objective = m_solver->MutableObjective();

    // Latency Penalty Term
    for (size_t i = 0; i < requests.size(); ++i) {
        for (size_t a = 0; a < graph.get_num_edges(); ++a) {
            double latency = graph.get_edge(a).latency;
            double penalty = -1.0 * (m_max_order_profit * (latency / m_max_latency));
            objective->SetCoefficient(flow_vars[i * graph.get_num_edges() + a], penalty);
        }
    }

    // Cost Term
    for (size_t a = 0; a < graph.get_num_edges(); ++a) {
        double cost_a = graph.get_edge(a).lease_cost;
        objective->SetCoefficient(edge_vars[a], -cost_a);
    }

    objective->SetMaximization();
    return objective;
}

double MILPSolver::solve(const HFT::Graph& graph, const HFT::ExpectedRequests& requests) {
    if (!m_solver) {
        LOG(ERROR) << "No solver has been set. Please set the solver and try again.";
    }

    std::vector<or_tools::MPVariable*> edge_vars{ build_edge_variables(graph, requests) };
    std::vector<or_tools::MPVariable*> flow_vars{ build_flow_variables(graph, requests) };

    apply_flow_conservation_constraints(graph, requests, flow_vars);
    apply_capacity_constraints(graph, requests, flow_vars, edge_vars);

    or_tools::MPObjective* const objective = build_objective_function(graph, requests, flow_vars, edge_vars);

    LOG(INFO) << "Solving with " << m_solver->SolverVersion();
    const or_tools::MPSolver::ResultStatus result_status = m_solver->Solve();

    LOG(INFO) << "Status: " << result_status;

    switch (result_status) {
        case or_tools::MPSolver::OPTIMAL:
            LOG(INFO) << "The problem does have an optimal solution!";
            break;
        case or_tools::MPSolver::FEASIBLE:
            LOG(INFO) << "A potentially suboptimal solution was found";
            break;
        default:
            LOG(WARNING) << "The solver could not solve the problem.";
            return 0.0;
    }

    double total_profit = objective->Value();
    for (const auto& request : requests) {
        total_profit += m_max_order_profit * request.num_orders;
    }

    return total_profit;
}