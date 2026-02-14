#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

#include "types/ga_config.h"
#include "types/milp_config.h"

namespace HFTTypes {
    struct Config {
        std::string graph_file_path{};
        std::string expected_requests_path{};
        std::string algorithm{};

        int max_order_profit{};
        double max_latency{};

        GAConfig ga{};
        MILPConfig milp{};
    };
}

#endif