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

/**
 * A Mixed-Integer Linear Programming (MILP) solver.
 * 
 * This class models the problem as a Fixed-Charge Capacitated Network Design Problem (FCCNDP) with binary 
 * edge-leasing variables, aiming to maximise profit while satisfying rate-limit and latency constraints.
 */
class MILPSolver : public Solver {
public:
    /**
     * @param graph The network topology.
     * @param requests The set of order opportunity requests to be routed.
     * @param config MILP-specific parameters (solver id, etc.).
     * @param max_latency The maximum acceptable latency considered acceptable by the firm.
     * @param record_selected_edges Whether to log final edge selections.
     */
    MILPSolver(const HFT::Graph& graph, 
               const HFT::ExpectedRequests& requests,
               const HFT::MILPConfig& config,
               double max_latency,
               bool record_selected_edges); 

    double solve() override;
    void set_solver(const std::string& solver_id);

    /**
     * Constructs the Linear Programming (LP) model by initialising variables, 
     * constraints, and the objective function.
     */
    void build();
    
private:
    void build_edge_variables();
    void build_flow_variables();
    void apply_flow_conservation_constraints();                         
    void apply_capacity_constraints();
    void build_objective_function();

    // Variables representing the binary decision to lease an edge (1 - leased, 0 - not leased).
    std::vector<or_tools::MPVariable*> m_edge_vars{};

    // Variables representing the volume of flow on each edge per request.
    std::vector<or_tools::MPVariable*> m_flow_vars{};

    or_tools::MPObjective* m_objective_func;

    std::unique_ptr<or_tools::MPSolver> m_solver;
    HFT::MILPConfig m_config{};
};

#endif