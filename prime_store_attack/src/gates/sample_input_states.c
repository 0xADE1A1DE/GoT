#include <stdlib.h>
#include "gates/sample_input_states.h"

void __sample_helper(gate_results *results, address_state state, int i) {
    if (state == IN_CACHE)
        results->inputs[i] = get_rand_range(L1, LLC);
    else
        results->inputs[i] = RAM;
}

void _sample_helper(gate_metadata *metadata, gate_results *results, address_state initial) {
    address_state other_state = (initial == IN_CACHE) ? RAM : IN_CACHE;
    for (int i = 0; i < metadata->inputs_amount; i++) {
        __sample_helper(results, initial, i);
    }
    bool should_flip = get_rand_range(0, 1);
    if (should_flip) {
        int i = get_rand_range(0, metadata->inputs_amount - 1);
        __sample_helper(results, other_state, i);
    }
}

void fan_sample_inputs(gate_metadata *metadata, gate_results *results) {
    // Initial state doesn't really matter.
    return _sample_helper(metadata, results, IN_CACHE);
}
void and_fan_sample_inputs(gate_metadata *metadata, gate_results *results) {
    return _sample_helper(metadata, results, IN_CACHE);
}
void or_fan_sample_inputs(gate_metadata *metadata, gate_results *results) {
    return _sample_helper(metadata, results, RAM);
}

void not_fan_sample_inputs(gate_metadata *metadata, gate_results *results) {
    // Initial state doesn't really matter.
    return _sample_helper(metadata, results, IN_CACHE);
}
void nand_fan_sample_inputs(gate_metadata *metadata, gate_results *results) {
    return _sample_helper(metadata, results, IN_CACHE);
}
void nor_fan_sample_inputs(gate_metadata *metadata, gate_results *results) {
    return _sample_helper(metadata, results, RAM);
}