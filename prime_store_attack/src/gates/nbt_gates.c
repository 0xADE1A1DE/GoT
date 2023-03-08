#include <stdlib.h>
#include <stdbool.h>
#include "gates/nbt_gates.h"
#include "gates/simulations.h"
#include "gates/sample_input_states.h"
#include "util.h"

typedef uintptr_t (*nbt_read_outputs_impl_func_type)(int in_speculative_execution, gate_bank *bank, float non_temporal_zero);
typedef uintptr_t (*nbt_read_outputs_func_type)(int index, gate_bank *bank, float non_temporal_zero, int temporal_zero);

uintptr_t __always_inline nbt_temporal_branch_misprediction(int index, gate_bank *bank, float non_temporal_zero, int temporal_zero, nbt_read_outputs_impl_func_type read_outputs) {
    index = index & 0xf;
    switch (index + temporal_zero) {
        case 0x0: asm volatile(""); return read_outputs(index - 0x0, bank, non_temporal_zero);
        case 0x1: asm volatile(""); return read_outputs(index - 0x1, bank, non_temporal_zero);
        case 0x2: asm volatile(""); return read_outputs(index - 0x2, bank, non_temporal_zero);
        case 0x3: asm volatile(""); return read_outputs(index - 0x3, bank, non_temporal_zero);
        case 0x4: asm volatile(""); return read_outputs(index - 0x4, bank, non_temporal_zero);
        case 0x5: asm volatile(""); return read_outputs(index - 0x5, bank, non_temporal_zero);
        case 0x6: asm volatile(""); return read_outputs(index - 0x6, bank, non_temporal_zero);
        case 0x7: asm volatile(""); return read_outputs(index - 0x7, bank, non_temporal_zero);
        case 0x8: asm volatile(""); return read_outputs(index - 0x8, bank, non_temporal_zero);
        case 0x9: asm volatile(""); return read_outputs(index - 0x9, bank, non_temporal_zero);
        case 0xa: asm volatile(""); return read_outputs(index - 0xa, bank, non_temporal_zero);
        case 0xb: asm volatile(""); return read_outputs(index - 0xb, bank, non_temporal_zero);
        case 0xc: asm volatile(""); return read_outputs(index - 0xc, bank, non_temporal_zero);
        case 0xd: asm volatile(""); return read_outputs(index - 0xd, bank, non_temporal_zero);
        case 0xe: asm volatile(""); return read_outputs(index - 0xe, bank, non_temporal_zero);
        case 0xf: asm volatile(""); return read_outputs(index - 0xf, bank, non_temporal_zero);
    }
}

// TODO: consider allowing the inlining of _nbt_read_##output_fan##outputs?

#define MAKE_READ_OUTPUTS_FUNC(output_fan) \
uintptr_t __attribute__ ((noinline)) _nbt_read_##output_fan##outputs_impl(int in_speculative_execution, gate_bank *bank, float non_temporal_zero) {  \
    if (!in_speculative_execution)                                                                                                                   \
        return 0;                                                                                                                                    \
    return read_outputs_base(bank, non_temporal_zero, output_fan);                                                                                   \
}                                                                                                                                                    \
uintptr_t __attribute__ ((noinline)) _nbt_read_##output_fan##outputs(int index, gate_bank *bank, float non_temporal_zero, int temporal_zero) {       \
    return nbt_temporal_branch_misprediction(index, bank, non_temporal_zero, temporal_zero, _nbt_read_##output_fan##outputs_impl);                   \
}

MAKE_READ_OUTPUTS_FUNC(1)
MAKE_READ_OUTPUTS_FUNC(2)
MAKE_READ_OUTPUTS_FUNC(3)
MAKE_READ_OUTPUTS_FUNC(4)
MAKE_READ_OUTPUTS_FUNC(5)
MAKE_READ_OUTPUTS_FUNC(6)
MAKE_READ_OUTPUTS_FUNC(7)
MAKE_READ_OUTPUTS_FUNC(8)
MAKE_READ_OUTPUTS_FUNC(9)
MAKE_READ_OUTPUTS_FUNC(10)
MAKE_READ_OUTPUTS_FUNC(11)
MAKE_READ_OUTPUTS_FUNC(12)
MAKE_READ_OUTPUTS_FUNC(13)
MAKE_READ_OUTPUTS_FUNC(14)

const nbt_read_outputs_func_type read_outputs_funcs_array[] = {
    NULL,
    _nbt_read_1outputs,
    _nbt_read_2outputs,
    _nbt_read_3outputs,
    _nbt_read_4outputs,
    _nbt_read_5outputs,
    _nbt_read_6outputs,
    _nbt_read_7outputs,
    _nbt_read_8outputs,
    _nbt_read_9outputs,
    _nbt_read_10outputs,
    _nbt_read_11outputs,
    _nbt_read_12outputs,
    _nbt_read_13outputs,
    _nbt_read_14outputs,
};

uint16_t counter = 0;

uintptr_t __always_inline _nbt_nand_base(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fan) {
    // Read the counter, to ensure it's in cache.
    trash ^= counter;
    bank = ensure_bank_loaded(bank, trash);

    // Make sure the bank lands, or introduce some ROB distance, not quite sure why this is so effective.
    for (int i = 0; i < 32; i++) {asm volatile("");}

    bool compare_with = read_inputs_nand_base(bank, input_fanout, &trash);

    uintptr_t result = read_outputs_funcs_array[output_fan](counter++, bank, wet_run - 1, !compare_with);
    return trash + result;
}

uintptr_t __always_inline _nbt_nand(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fanout) {
    return _nbt_nand_base(wet_run, bank, trash, input_fanout, output_fanout);
}
uintptr_t __always_inline _nbt_not(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fanout) {
    return _nbt_nand_base(wet_run, bank, trash, 1, output_fanout);
}

#define DEFINE_NBT_GATE(operation, input_fanout, output_fanout) DEFINE_GATE(nbt, operation, false, input_fanout, output_fanout, "no branch training")

DEFINE_NBT_GATE(nand, 2, 1);
DEFINE_NBT_GATE(nand, 3, 1);
DEFINE_NBT_GATE(nand, 4, 1);
DEFINE_NBT_GATE(nand, 5, 1);
DEFINE_NBT_GATE(nand, 6, 1);
DEFINE_NBT_GATE(nand, 7, 1);
DEFINE_NBT_GATE(nand, 8, 1);
DEFINE_NBT_GATE(nand, 9, 1);
DEFINE_NBT_GATE(nand, 10, 1);
DEFINE_NBT_GATE(nand, 11, 1);
DEFINE_NBT_GATE(nand, 12, 1);
DEFINE_NBT_GATE(nand, 13, 1);
DEFINE_NBT_GATE(nand, 14, 1);

DEFINE_NBT_GATE(not, 1, 1);
DEFINE_NBT_GATE(not, 1, 2);
DEFINE_NBT_GATE(not, 1, 3);
DEFINE_NBT_GATE(not, 1, 4);
DEFINE_NBT_GATE(not, 1, 5);
DEFINE_NBT_GATE(not, 1, 6);
DEFINE_NBT_GATE(not, 1, 7);
DEFINE_NBT_GATE(not, 1, 8);
DEFINE_NBT_GATE(not, 1, 9);
DEFINE_NBT_GATE(not, 1, 10);
DEFINE_NBT_GATE(not, 1, 11);
DEFINE_NBT_GATE(not, 1, 12);
DEFINE_NBT_GATE(not, 1, 13);
DEFINE_NBT_GATE(not, 1, 14);

DEFINE_NBT_GATE(nand, 12, 2);