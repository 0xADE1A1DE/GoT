#pragma once
#include <math.h>
#include "gates.h"

namespace gates {

void __always_inline ensure_bank_loaded(GateBank *&bank, uint64_t trash) {
    // Make sure future bank dereferences depend on `trash`
    bank = (GateBank *)((uintptr_t)bank | (trash == 0xbaaaaad) | (bank->inputs[0] == 0xbaaaaad) | (bank->outputs[0] == 0xbaaaaad));
}

#ifndef WASM
// The optimal value found through binary search - 94.
#define DEFAULT_OVERWRITE_BRANCH_HISTORY 100
#else
// Chrome has default loop unrolling of 4, I couldn't find a a way around that.
// This value wasn't fully optimized, but some steps of manual binary search were taken.
#define DEFAULT_OVERWRITE_BRANCH_HISTORY 282
#endif
void __always_inline overwrite_branch_history() {
    #pragma nounroll
    for (uint64_t i = 0; i < DEFAULT_OVERWRITE_BRANCH_HISTORY; i++) {asm volatile("");}
}

// This function always returns 1 (unless the value is 0xbaaaaad).
bool __always_inline read_input(GateBank *bank, int index) {
    return (READ(bank->inputs[index])) != 0xbaaaaad;
}

// This function always returns 1 (unless one the values is 0xbaaaaad).
bool __always_inline read_inputs_and_base(GateBank *bank, int fan_in, uint64_t &trash) {
    bool and_result = 1;
    #pragma clang loop unroll(full)
    for (int i = 0; i < fan_in; i++) {
        and_result &= read_input(bank, i);
    }
    trash ^= and_result;
    return and_result;
}

void __always_inline prevent_reorder() { asm volatile (""); }

// NOTE: `slowdown_parmater` is an important paramater, be careful when playing with it!
// It balances between ensuring speculative windows induced by LLC reads don't reach the reading outputs part,
// and between having the last output read and the first input read within the same ROB (i.e, not too far from eachother).
// NOTE: It may be the case that this paramater should be tuned with respect to the amount on inputs in the gate.
double __always_inline slowdown_chain(double non_temporal_zero, const int slowdown_parmater = 14) {
    #pragma clang loop unroll(full)
    for (int i = 0; i < slowdown_parmater; i++) {non_temporal_zero = sqrt(non_temporal_zero);}

    return non_temporal_zero;
}

uint64_t __always_inline read_outputs_base(GateBank *bank, int fan_out, uintptr_t temporal_zero) {
    // Make sure future bank dereferences depend on `non_temporal_zero`
    bank = (GateBank *)((uintptr_t)bank | (temporal_zero));

    uint64_t sum = 0;
    #pragma clang loop unroll(full)
    for (int i = 0; i < fan_out; i++) {
        sum += *(uint64_t *)(bank->outputs[i]);
    }
    return sum;
}

static uint64_t branch_counter = 0;

#ifndef SWITCH_SIZE
#define SWITCH_SIZE 2
#endif

// This function returns 1 iff the execution is within a speculative window.
static uintptr_t __attribute__ ((noinline)) temporal_branch_misprediction(uint64_t temporal_zero) {
    uint64_t index = (branch_counter++) % SWITCH_SIZE;
    switch (index ^ temporal_zero) {
        case 0x0: asm volatile(""); if (index == 0x0) return 0; break;
        case 0x1: asm volatile(""); if (index == 0x1) return 0; break;
        case 0x2: asm volatile(""); if (index == 0x2) return 0; break;
        case 0x3: asm volatile(""); if (index == 0x3) return 0; break;
        case 0x4: asm volatile(""); if (index == 0x4) return 0; break;
        case 0x5: asm volatile(""); if (index == 0x5) return 0; break;
        case 0x6: asm volatile(""); if (index == 0x6) return 0; break;
        case 0x7: asm volatile(""); if (index == 0x7) return 0; break;
        case 0x8: asm volatile(""); if (index == 0x8) return 0; break;
        case 0x9: asm volatile(""); if (index == 0x9) return 0; break;
        case 0xa: asm volatile(""); if (index == 0xa) return 0; break;
        case 0xb: asm volatile(""); if (index == 0xb) return 0; break;
        case 0xc: asm volatile(""); if (index == 0xc) return 0; break;
        case 0xd: asm volatile(""); if (index == 0xd) return 0; break;
        case 0xe: asm volatile(""); if (index == 0xe) return 0; break;
        case 0xf: asm volatile(""); if (index == 0xf) return 0; break;
    }
    return 1;
}

}