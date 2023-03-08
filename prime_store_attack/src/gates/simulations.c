#include "gates/simulations.h"

bool _is_in_cache(address_state state) {
    return (state <= LLC) || (state == IN_CACHE);
}

bool _and_inputs(gate_metadata *metadata, gate_results *results) {
    bool result = true;
    for (int i = 0; i < metadata->inputs_amount; i++) {
        result = result && _is_in_cache(results->inputs[i]);
    }
    return result;
}

bool _or_inputs(gate_metadata *metadata, gate_results *results) {
    bool result = false;
    for (int i = 0; i < metadata->inputs_amount; i++) {
        result = result || _is_in_cache(results->inputs[i]);
    }
    return result;
}

void _apply_fan_on_output(gate_metadata *metadata, gate_results *results, address_state result) {
    for (int i = 0; i < metadata->outputs_amount; i++)
        results->outputs[i] = result;
}

void fan_simulation(gate_metadata *metadata, gate_results *results) {
    _apply_fan_on_output(metadata, results, (_is_in_cache(results->inputs[0])) ? IN_CACHE: RAM);
}

void and_fan_simulation(gate_metadata *metadata, gate_results *results) {
    _apply_fan_on_output(metadata, results, _and_inputs(metadata, results) ? IN_CACHE: RAM);
}

void or_fan_simulation(gate_metadata *metadata, gate_results *results) {
    _apply_fan_on_output(metadata, results, _or_inputs(metadata, results) ? IN_CACHE: RAM);
}

void not_fan_simulation(gate_metadata *metadata, gate_results *results) {
    _apply_fan_on_output(metadata, results, (_is_in_cache(results->inputs[0])) ? RAM : IN_CACHE);
}

void nand_fan_simulation(gate_metadata *metadata, gate_results *results) {
    _apply_fan_on_output(metadata, results, _and_inputs(metadata, results) ? RAM : IN_CACHE);
}

void nor_fan_simulation(gate_metadata *metadata, gate_results *results) {
    _apply_fan_on_output(metadata, results, _or_inputs(metadata, results) ? RAM : IN_CACHE);
}