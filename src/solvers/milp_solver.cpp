#include "solvers/milp_solver.h"

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
#include "types/solver.h"

MILPSolver::MILPSolver(const HFT::Graph& graph, 
                       const HFT::ExpectedRequests& requests, 
                       const HFT::MILPConfig& config,
                       double max_latency) 
: Solver{ graph, requests, max_latency }
, m_config{ config } {
    set_solver(config.solver_id);
}

double MILPSolver::solve() {
    if (!m_solver) {
        LOG(ERROR) << "No solver has been set. Please set the solver and try again.";
        return -1;
    }

    if (!m_objective_func) {
        LOG(ERROR) << "No objective function defined.";
        return -1;
    }

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
            return std::numeric_limits<double>::lowest();
    }

    double total_profit = m_objective_func->Value();
    for (const auto& request : m_requests) {
        total_profit += request.max_order_profit * request.num_orders;
    }

    return total_profit;
}

void MILPSolver::set_solver(const std::string& solver_id) {
    std::unique_ptr<or_tools::MPSolver> new_solver{ or_tools::MPSolver::CreateSolver(solver_id) };
    if (!new_solver) {
        LOG(WARNING) << "Could not create solver " << solver_id;
        return;
    }

    m_solver = std::move(new_solver);
    m_config.solver_id = solver_id;
    build();
}

void MILPSolver::build() {
    if (m_solver) {
        build_edge_variables();
        build_flow_variables();

        apply_flow_conservation_constraints();
        apply_capacity_constraints();

        build_objective_function();
    }
}

void MILPSolver::build_edge_variables() {
    for (std::size_t i = 0; i < m_graph.get_num_edges(); ++i) {
        std::string edge_label = "y" + std::to_string(i);
        m_edge_vars.emplace_back(m_solver->MakeBoolVar(edge_label));
    }
}

void MILPSolver::build_flow_variables() {
    for (size_t i = 0; i < m_requests.size(); ++i) {
        for (size_t a = 0; a < m_graph.get_num_edges(); ++a) {
            std::string flow_label = "f" + std::to_string(i) + "," + std::to_string(a);
            m_flow_vars.emplace_back(m_solver->MakeNumVar(0.0, m_requests[i].num_orders, flow_label));
        }
    }
}

void MILPSolver::apply_flow_conservation_constraints() {
    for (size_t i = 0; i < m_requests.size(); ++i) {
        const auto& req = m_requests[i];
        
        for (size_t v = 0; v < m_graph.get_num_nodes(); ++v) {
            // Set the RHS (Right Hand Side) based on if v is a server, exchange, or an intermediate node
            double rhs = v == req.server ? req.num_orders : (v == req.exchange ? -req.num_orders : 0);
            
            // Create the constraint: sum(out) - sum(in) = rhs
            or_tools::MPConstraint* const flow_con = m_solver->MakeRowConstraint(rhs, rhs);
            const auto& node = m_graph.get_node(v);

            // Outgoing edges: sum(f_{i,a})
            for (const auto& edge_id : node.outgoing_edges) {
                flow_con->SetCoefficient(m_flow_vars[i * m_graph.get_num_edges() + edge_id], 1.0);
            }

            // Incoming edges: -sum(f_{i,a})
            for (const auto& edge_id : node.incoming_edges) {
                flow_con->SetCoefficient(m_flow_vars[i * m_graph.get_num_edges() + edge_id], -1.0);
            }
        }
    }
}

void MILPSolver::apply_capacity_constraints() {
    for (size_t i = 0; i < m_requests.size(); ++i) {
        for (size_t a = 0; a < m_graph.get_num_edges(); ++a) {
            or_tools::MPConstraint* const cap_con = m_solver->MakeRowConstraint(-or_tools::MPSolver::infinity(), 0.0);
            double capacity_coeff = m_graph.get_edge(a).rate_limit * m_requests[i].planning_horizon;
            
            cap_con->SetCoefficient(m_flow_vars[i * m_graph.get_num_edges() + a], 1.0);
            cap_con->SetCoefficient(m_edge_vars[a], -capacity_coeff);
        }
    }
}

void MILPSolver::build_objective_function() {
    m_objective_func = m_solver->MutableObjective();

    // Latency Penalty Term
    for (size_t i = 0; i < m_requests.size(); ++i) {
        for (size_t a = 0; a < m_graph.get_num_edges(); ++a) {
            double edge_latency = m_graph.get_edge(a).latency;
            double penalty = -1.0 * (m_requests[i].max_order_profit * (edge_latency / m_max_latency));
            m_objective_func->SetCoefficient(m_flow_vars[i * m_graph.get_num_edges() + a], penalty);
        }
    }

    // Cost Term
    for (size_t a = 0; a < m_graph.get_num_edges(); ++a) {
        double cost_a = m_graph.get_edge(a).lease_cost;
        m_objective_func->SetCoefficient(m_edge_vars[a], -cost_a);
    }

    m_objective_func->SetMaximization();
}