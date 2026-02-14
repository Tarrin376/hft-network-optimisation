#ifndef MILP_CONFIG_H
#define MILP_CONFIG_H

#include <string>

namespace HFTTypes {
    struct MILPConfig {
        std::string solver_id{};
    };
}

#endif