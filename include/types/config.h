#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <cstdint>

#include "types/ga_config.h"

#include "types/milp_config.h"

namespace HFT {
    struct Config {
        std::string nodes_file_name{};
        std::string edges_file_name{};
        std::string requests_file_name{};
        std::string recorded_edges_file_name{};
        std::string algorithm{};
        
        int num_shortest_paths{};
        double max_latency{};

        GAConfig ga{};
        MILPConfig milp{};
    };
}

#endif