#include "types/solver.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <functional>

#include "types/expected_requests.h"
#include "types/graph.h"

Solver::Solver(const HFT::Graph& graph, const HFT::ExpectedRequests& requests, double max_latency, bool record_selected_edges) 
    : m_graph{ graph }
    , m_requests{ requests }
    , m_max_latency{ max_latency }
    , m_record_selected_edges{ record_selected_edges } {}

std::vector<std::reference_wrapper<const HFT::Edge>> Solver::get_selected_edges() const {
    auto edge_mapper = [this](auto edge_id) -> std::reference_wrapper<const HFT::Edge> {
        return std::ref(m_graph.get_edge(edge_id));
    };

    std::vector<std::reference_wrapper<const HFT::Edge>> mapped_edges{};
    mapped_edges.reserve(m_selected_edges.size());

    // Transform selected edge IDs into a collection of reference wrappers to 
    // provide a view into the graph without copying edge structures.
    std::transform(
        m_selected_edges.begin(), 
        m_selected_edges.end(), 
        std::back_inserter(mapped_edges), 
        edge_mapper);
    
    return mapped_edges;
}