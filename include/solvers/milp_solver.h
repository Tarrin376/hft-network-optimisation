#ifndef MILP_SOLVER
#define MILP_SOLVER

#include <string>
#include <memory>
#include <vector>

#include "ortools/linear_solver/linear_solver.h"

#include "types/expected_requests.h"
#include "types/milp_config.h"
#include "types/graph.h"

#include "solvers/solver.h"

namespace or_tools = operations_research;

class MILPSolver : public Solver {
public:
    MILPSolver(int max_order_profit, double max_latency, const HFT::MILPConfig& config);

    double solve(const HFT::Graph& graph, const HFT::ExpectedRequests& requests);

    void set_solver(const std::string& solver_id);

    std::vector<or_tools::MPVariable*> build_edge_variables(const HFT::Graph& graph, 
                                                            const HFT::ExpectedRequests& requests);
    
    std::vector<or_tools::MPVariable*> build_flow_variables(const HFT::Graph& graph, 
                                                            const HFT::ExpectedRequests& requests);

    void apply_flow_conservation_constraints(const HFT::Graph& graph, 
                                             const HFT::ExpectedRequests& requests,
                                             const std::vector<or_tools::MPVariable*> flow_vars);
                                    
    void apply_capacity_constraints(const HFT::Graph& graph, 
                                    const HFT::ExpectedRequests& requests,
                                    const std::vector<or_tools::MPVariable*> flow_vars,
                                    const std::vector<or_tools::MPVariable*> edge_vars);

    or_tools::MPObjective* const build_objective_function(const HFT::Graph& graph, 
                                                          const HFT::ExpectedRequests& requests,
                                                          const std::vector<or_tools::MPVariable*> flow_vars,
                                                          const std::vector<or_tools::MPVariable*> edge_vars);
private:
    std::unique_ptr<or_tools::MPSolver> m_solver;
    HFT::MILPConfig m_config{};
};

#endif