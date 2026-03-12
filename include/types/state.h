#ifndef STATE_H
#define STATE_H

#include <cstdint>

namespace HFT {
    struct State {
        double latency;
        std::size_t node_id;

        State(double l, std::size_t id) : latency{ l }, node_id{ id } {}

        bool operator>(const State& other) const {
            return latency > other.latency;
        }
    };
}

#endif