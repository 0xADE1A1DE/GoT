#include "gates/gates.h"
#include "consts.h"


uintptr_t apply_gate(gate_metadata *metadata, gate_bank *bank, uintptr_t trash) {
    static gate_bank __attribute__ ((aligned (64))) fake = {0};
    fake.input_base = (uintptr_t)bank;
    if (metadata->requires_branch_training) {
        trash = metadata->gate_func(0, &fake, trash);
        trash = metadata->gate_func(0, &fake, trash);
    }
    trash = metadata->gate_func(1, bank, trash);
    return trash;
}