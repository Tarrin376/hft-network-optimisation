#ifndef MILP_SOLVER
#define MILP_SOLVER

#include <string>
#include <memory>
#include <vector>

#include "ortools/linear_solver/linear_solver.h"

#include "types/expected_requests.h"
#include "types/milp_config.h"
#include "types/graph.h"
#include "types/solver.h"

namespace or_tools = operations_research;

class MILPSolver : public Solver {
public:
    MILPSolver(const HFT::Graph& graph, 
               const HFT::ExpectedRequests& requests,
               const HFT::MILPConfig& config,
               double max_latency); 

    double solve() override;
    void set_solver(const std::string& solver_id);
    void build();
    
private:
    void build_edge_variables();
    void build_flow_variables();
    void apply_flow_conservation_constraints();                         
    void apply_capacity_constraints();
    void build_objective_function();

    std::vector<or_tools::MPVariable*> m_edge_vars{};
    std::vector<or_tools::MPVariable*> m_flow_vars{};

    or_tools::MPObjective* m_objective_func;

    std::unique_ptr<or_tools::MPSolver> m_solver;
    HFT::MILPConfig m_config{};
};

#endif