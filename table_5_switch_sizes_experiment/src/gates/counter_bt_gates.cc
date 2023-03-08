#include "gates/counter_bt_gates.h"
#include "gates/gates_common.h"
#include <math.h>

namespace gates {

template<int fan_in, int fan_out>
uint64_t CounterBranchTrainingNand<fan_in, fan_out>::gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const {
    // Read the counter, to ensure it's in cache.
    trash &= branch_counter;
    ensure_bank_loaded(bank, trash);

    // Make sure the bank lands, or introduce some ROB distance, not quite sure why this is so effective.
    #pragma nounroll
    for (uint64_t i = 0; i < 32; i++) {asm volatile("");}

    bool compare_with = read_inputs_and_base(bank, fan_in, trash);

    if (!temporal_branch_misprediction(!compare_with))
        return trash;

    // This code only runs within a speculative window.
    double after_slowdown = slowdown_chain(wet_run - 1);
    return trash + read_outputs_base(bank, fan_out, after_slowdown != 0);
}

template<int fan_in, int fan_out>
uint64_t CounterBranchTrainingAnd<fan_in, fan_out>::gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const {
    // Read the counter, to ensure it's in cache.
    trash &= branch_counter;
    ensure_bank_loaded(bank, trash);

    // Make sure the bank lands, or introduce some ROB distance, not quite sure why this is so effective.
    #pragma nounroll
    for (uint64_t i = 0; i < 64; i++) {asm volatile("");}

    double result = slowdown_chain(1 - wet_run, 17);

    if (!temporal_branch_misprediction(result != 0.0))
        return trash;
    
    // This code will only run in the wet run within a speculative window.
    read_inputs_and_base(bank, fan_in, trash);
    return read_outputs_base(bank, fan_out, (trash == 0xbaaaaad));
}


constexpr counter_gates::counter_gates() {
    int i = 0;
    for_values<12>([&]<auto fan_in>() {
        for_values<1>([&]<auto fan_out>() {
            gates[i++] = new CounterBranchTrainingNand<fan_in, fan_out>();
        });
    });
    for_values<2>([&]<auto fan_out>() {
        gates[i++] = new CounterBranchTrainingNot<fan_out>();
    });

    // for_values<2, 4, 8, 11, 12>([&]<auto fan_in>() {
    //     for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
    //         gates[i++] = new CounterBranchTrainingNand<fan_in, fan_out>();
    //     });
    // });
    // for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
    //     gates[i++] = new CounterBranchTrainingNot<fan_out>();
    // });

    // for_values<2, 4, 8, 11, 12>([&]<auto fan_in>() {
    //     for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
    //         gates[i++] = new CounterBranchTrainingAnd<fan_in, fan_out>();
    //     });
    // });
    // for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
    //     gates[i++] = new CounterBranchTrainingBuffer<fan_out>();
    // });
}

counter_gates counter_bt_gates_to_test;

}