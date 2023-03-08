#include <stdbool.h>
#include "gates/gates_common.h"
#include "gates/bt_gates.h"
#include "gates/simulations.h"
#include "gates/sample_input_states.h"
#include "util.h"

uintptr_t __always_inline _bt_nand_base(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fanout) {
    // Make sure future bank dereferences depend on `trash`
    bank = ensure_bank_loaded(bank, trash);
    
    // branch prediction training.
    for (int i = 0; i < 128; i++) {asm volatile("");}

    bool compare_with = read_inputs_nand_base(bank, input_fanout, &trash);

    if (wet_run == compare_with) {
        return trash;
    }

    // This is needed to stop the compiler from re-ordering the if statements.
    asm volatile("");
    if (!wet_run)
        return trash;

    // If properly trained, we now have a speculative window.
    return trash + read_outputs_base(bank, wet_run - 1, output_fanout);
}


// NOTE: unlike nand, nor doesn't guarantee that all inputs would be read.
uintptr_t __always_inline _bt_nor_base(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fanout) {
    // Make sure future bank dereferences depend on `trash`
    bank = ensure_bank_loaded(bank, trash);
    
    // branch prediction training.
    for (int i = 0; i < 128; i++) {asm volatile("");}

    for (int i = 0; i < input_fanout; i++) {
        if (wet_run == read_input(bank, i)) {
            return trash;
        }
    }

    // This is needed to stop the compiler from re-ordering the if statements.
    asm volatile("");
    if (!wet_run)
        return trash;

    // If properly trained, we now have a speculative window.
    return trash + read_outputs_base(bank, wet_run - 1, output_fanout);
}

uintptr_t __always_inline _bt_nand(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fanout) {
    return _bt_nand_base(wet_run, bank, trash, input_fanout, output_fanout);
}
uintptr_t __always_inline _bt_nor(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fanout) {
    return _bt_nor_base(wet_run, bank, trash, input_fanout, output_fanout);
}
uintptr_t __always_inline _bt_not(int wet_run, gate_bank *bank, uintptr_t trash, int input_fanout, int output_fanout) {
    return _bt_nand_base(wet_run, bank, trash, 1, output_fanout);
}

#define DEFINE_BT_GATE(operation, input_fanout, output_fanout) DEFINE_GATE(bt, operation, true, input_fanout, output_fanout, "branch training")

DEFINE_BT_GATE(nand, 2, 1);
DEFINE_BT_GATE(nand, 3, 1);
DEFINE_BT_GATE(nand, 4, 1);
DEFINE_BT_GATE(nand, 5, 1);
DEFINE_BT_GATE(nand, 6, 1);
DEFINE_BT_GATE(nand, 7, 1);
DEFINE_BT_GATE(nand, 8, 1);
DEFINE_BT_GATE(nand, 9, 1);
DEFINE_BT_GATE(nand, 10, 1);
DEFINE_BT_GATE(nand, 11, 1);
DEFINE_BT_GATE(nand, 12, 1);
DEFINE_BT_GATE(nand, 13, 1);
DEFINE_BT_GATE(nand, 14, 1);

DEFINE_BT_GATE(nor, 2, 1);
DEFINE_BT_GATE(nor, 3, 1);
DEFINE_BT_GATE(nor, 4, 1);
DEFINE_BT_GATE(nor, 5, 1);
DEFINE_BT_GATE(nor, 6, 1);
DEFINE_BT_GATE(nor, 7, 1);
DEFINE_BT_GATE(nor, 8, 1);
DEFINE_BT_GATE(nor, 9, 1);
DEFINE_BT_GATE(nor, 10, 1);
DEFINE_BT_GATE(nor, 11, 1);
DEFINE_BT_GATE(nor, 12, 1);
DEFINE_BT_GATE(nor, 13, 1);
DEFINE_BT_GATE(nor, 14, 1);

DEFINE_BT_GATE(not, 1, 1);
DEFINE_BT_GATE(not, 1, 2);
DEFINE_BT_GATE(not, 1, 3);
DEFINE_BT_GATE(not, 1, 4);
DEFINE_BT_GATE(not, 1, 5);
DEFINE_BT_GATE(not, 1, 6);
DEFINE_BT_GATE(not, 1, 7);
DEFINE_BT_GATE(not, 1, 8);
DEFINE_BT_GATE(not, 1, 9);
DEFINE_BT_GATE(not, 1, 10);
DEFINE_BT_GATE(not, 1, 11);
DEFINE_BT_GATE(not, 1, 12);
DEFINE_BT_GATE(not, 1, 13);
DEFINE_BT_GATE(not, 1, 14);

DEFINE_BT_GATE(nand, 12, 2);