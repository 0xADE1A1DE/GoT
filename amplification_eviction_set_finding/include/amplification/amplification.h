#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "consts.h"
#include "util.h"

namespace amplification {

class Amplification {
public:
    virtual void initialize(uint64_t &trash) = 0;
    virtual void amplify(uintptr_t target, uint64_t &trash) = 0;
    virtual bool is_in_cache(uint64_t time_difference, measure::measure_type type) = 0;
};

}