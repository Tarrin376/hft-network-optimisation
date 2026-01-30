#include <iostream>

#include "graph.h"

int main() {
    Graph graph{};
    graph.addEdge({ 1, 2, 3, 1, 2.1, 100 });

    const std::vector<int>& edges{ graph.outgoingEdges(2) };
    for (const auto edge : edges) {
        std::cout << edge << '\n';
    }

    return 0;
}