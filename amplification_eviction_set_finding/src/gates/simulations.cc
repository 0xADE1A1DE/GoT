#include "gates/gates.h"
#include "util.h"

namespace gates {

bool _is_in_cache(address_state state) {
    return (state <= LLC) || (state == IN_CACHE);
}

bool _and_inputs(GateResults &results, int fan_in) {
    bool result = true;
    for (int i = 0; i < fan_in; i++) {
        result = result && _is_in_cache(results.inputs[i]);
    }
    return result;
}

bool _or_inputs(GateResults &results, int fan_in) {
    bool result = false;
    for (int i = 0; i < fan_in; i++) {
        result = result || _is_in_cache(results.inputs[i]);
    }
    return result;
}

void _apply_fan_on_output(GateResults &results, int fan_out, address_state result) {
    for (int i = 0; i < fan_out; i++)
        results.outputs[i] = result;
}

void and_simulation(GateResults &results, int fan_in, int fan_out) {
    _apply_fan_on_output(results, fan_out, _and_inputs(results, fan_in) ? IN_CACHE : RAM);
}

void or_simulation(GateResults &results, int fan_in, int fan_out) {
    _apply_fan_on_output(results, fan_out, _or_inputs(results, fan_in) ? IN_CACHE : RAM);
}

void nand_simulation(GateResults &results, int fan_in, int fan_out) {
    _apply_fan_on_output(results, fan_out, _and_inputs(results, fan_in) ? RAM : IN_CACHE);
}

void nor_simulation(GateResults &results, int fan_in, int fan_out) {
    _apply_fan_on_output(results, fan_out, _or_inputs(results, fan_in) ? RAM : IN_CACHE);
}

}