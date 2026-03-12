#ifndef PATH_BASED_GA_CONFIG
#define PATH_BASED_GA_CONFIG

#include "ga_config.h"

namespace HFT {
    struct PathBasedGAConfig : public GAConfig {
        int k{ 10 };
    };
}

#endif