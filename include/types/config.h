#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

#include "types/ga_config.h"
#include "types/path_based_ga_config.h"
#include "types/milp_config.h"

namespace HFT {
    struct Config {
        std::string graph_file_path{};
        std::string expected_requests_path{};
        std::string algorithm{};

        double max_latency{};

        GAConfig ga{};
        PathBasedGAConfig path_ga{};
        MILPConfig milp{};
    };
}

#endif