#pragma once
#include "amplification/amplification.h"

namespace amplification {

class NoopAmplification : public Amplification {
public:
    void initialize(uint64_t &trash) final { }
    void amplify(uintptr_t target, uint64_t &trash) final { trash = FORCE_READ(target, trash); }
    bool is_in_cache(uint64_t time_difference, measure::measure_type type) final {
        if (type == measure::t_RDTSC)
            return time_difference < RDTSC_THRESHOLD;

        // This "amplification" cannot distinguish with t_100US.
        return false;
    }
};

// This is an amplification that does absolutely no amplification.
// The `amplify` method simply accesses the target.
extern Amplification &noop_amplification;

}