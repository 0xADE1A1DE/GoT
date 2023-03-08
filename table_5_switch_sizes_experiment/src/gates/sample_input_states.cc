#include "util.h"
#include "gates/gates.h"

namespace gates {

void __sample_helper(GateResults &results, address_state state, int i) {
    if (state == IN_CACHE)
        results.inputs[i] = (address_state)get_rand_range(L1, LLC);
    else
        results.inputs[i] = RAM;
}

void _sample_helper(GateResults &results, int fan_in, int fan_out, address_state initial) {
    address_state other_state = (initial == IN_CACHE) ? RAM : IN_CACHE;
    for (int i = 0; i < fan_in; i++) {
        __sample_helper(results, initial, i);
    }
    bool should_flip = get_rand_range(0, 1);
    if (should_flip) {
        int i = get_rand_range(0, fan_in - 1);
        __sample_helper(results, other_state, i);
    }
}

void and_sample_inputs(GateResults &results, int fan_in, int fan_out) {
    return _sample_helper(results, fan_in, fan_out, IN_CACHE);
}
void or_sample_inputs(GateResults &results, int fan_in, int fan_out) {
    return _sample_helper(results, fan_in, fan_out, RAM);
}

void nand_sample_inputs(GateResults &results, int fan_in, int fan_out) {
    return _sample_helper(results, fan_in, fan_out, IN_CACHE);
}
void nor_sample_inputs(GateResults &results, int fan_in, int fan_out) {
    return _sample_helper(results, fan_in, fan_out, RAM);
}

}