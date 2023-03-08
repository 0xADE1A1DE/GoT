#pragma once
#include "gates.h"

gate_bank __always_inline *ensure_bank_loaded(gate_bank *bank, uintptr_t trash) {
    return (gate_bank *)((uintptr_t)bank | (trash == 0xbaaaaad) | (bank->input_base == 0xbaaaaad) | (bank->output_base == 0xbaaaaad));
}

bool __always_inline read_input(gate_bank *bank, int index) {
    // This function always returns 1 (unless the value is some magic), but to find that out, the processor has to fetch the input.
    return (*(uintptr_t *)(bank->input_base + bank->input_offsets[index])) != 0xbaaaaad;
}

bool __always_inline read_inputs_nand_base(gate_bank *bank, int input_fanout, uintptr_t *trash) {
    bool and_result = 1;
    #pragma GCC unroll 300
    for (int i = 0; i < input_fanout; i++) {
        and_result &= read_input(bank, i);
    }
    *trash ^= and_result;
    return and_result;
}

// NOTE: This is an important paramater, be careful when playing with it!
// It balances between ensuring speculative windows induced by LLC reads don't reach the reading outputs part,
// and between having the last output read and the first input read within the same ROB (i.e, not too far from eachother).
// Indeed, with slower opcodes (we're relying on the compiler emitting `mulss`), it might be easier to strike this balance.
// NOTE: It may be the case that this paramater should be tuned with respect to the amount on inputs on the gate.
#define SPEC_SLOW_PARAM (42)

uintptr_t __always_inline read_outputs_base(gate_bank *bank, float non_temporal_zero, int output_fanout) {
    #pragma GCC unroll 300
    for (int i = 0; i < SPEC_SLOW_PARAM; i++) {non_temporal_zero *= non_temporal_zero;}

    // Make sure future bank dereferences depend on `non_temporal_zero`
    bank = (gate_bank *)((uintptr_t)bank | (non_temporal_zero != 0));

    uintptr_t sum = 0;
    #pragma GCC unroll 300
    for (int i = 0; i < output_fanout; i++) {
        sum += *(uintptr_t *)(bank->output_base + bank->output_offsets[i]);
    }
    return sum;
}

#define DEFINE_GATE(type, operation, branch_training, input_fanout, output_fanout, pretty_type_name)                \
uintptr_t type##_##operation##_##input_fanout##to##output_fanout(int wet_run, gate_bank *bank, uintptr_t trash) {   \
    return _##type##_##operation(wet_run, bank, trash, input_fanout, output_fanout);                                \
}                                                                                                                   \
gate_metadata type##_##operation##_##input_fanout##to##output_fanout##_metadata = {                                 \
    .inputs_amount = input_fanout,                                                                                  \
    .outputs_amount = output_fanout,                                                                                \
    .requires_branch_training = branch_training,                                                                    \
    .gate_func = type##_##operation##_##input_fanout##to##output_fanout,                                            \
    .sample_inputs_state = operation##_fan_sample_inputs,                                                           \
    .simulate_gate = operation##_fan_simulation,                                                                    \
    .gate_name = pretty_type_name " " #operation " " #input_fanout "to" #output_fanout                              \
}
