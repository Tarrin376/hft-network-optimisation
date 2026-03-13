#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <cstdint>

#include "types/ga_config.h"

#include "types/milp_config.h"

namespace HFT {
    struct Config {
        std::string graph_file_path{};
        std::string expected_requests_path{};
        std::string algorithm{};
        std::size_t k{};

        double max_latency{};

        GAConfig ga{};
        MILPConfig milp{};
    };
}

#endif