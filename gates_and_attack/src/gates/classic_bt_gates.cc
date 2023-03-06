#include "gates/classic_bt_gates.h"
#include "gates/gates_common.h"
#include <math.h>

namespace gates {

template<int fan_in, int fan_out>
uint64_t ClassicBranchTrainingNand<fan_in, fan_out>::gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const {
    ensure_bank_loaded(bank, trash);
    overwrite_branch_history();

    // This branch is trained in the dry runs.
    if (wet_run == read_inputs_and_base(bank, fan_in, trash)) {
        return trash;
    }

    prevent_reorder();
    if (!wet_run)
        return trash;

    // This code will only run in the wet run within a speculative window.
    double after_slowdown = slowdown_chain(wet_run - 1);
    return trash + read_outputs_base(bank, fan_out, after_slowdown != 0);
}

template<int fan_in, int fan_out>
uint64_t ClassicBranchTrainingNor<fan_in, fan_out>::gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const {
    ensure_bank_loaded(bank, trash);
    overwrite_branch_history();

    for (int i = 0; i < fan_in; i++) {
        // These branches are trained in the dry runs.
        if (wet_run == read_input(bank, i)) {
            return trash;
        }
    }

    prevent_reorder();
    if (!wet_run)
        return trash;

    // This code will only run in the wet run within a speculative window.
    double after_slowdown = slowdown_chain(wet_run - 1);
    return trash + read_outputs_base(bank, fan_out, after_slowdown != 0);
}

template<int fan_in, int fan_out>
uint64_t ClassicBranchTrainingAnd<fan_in, fan_out>::gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const {
    ensure_bank_loaded(bank, trash);
    overwrite_branch_history();

    double result = slowdown_chain(1 - wet_run, 19);

    // This branch is trained in the dry runs.
    if (result == 0.0)
        return trash;

    asm volatile("");
    if (!wet_run)
        return trash;

    // This code will only run in the wet run within a speculative window.
    read_inputs_and_base(bank, fan_in, trash);
    return read_outputs_base(bank, fan_out, (trash == 0xbaaaaad));
}

constexpr classic_gates::classic_gates() {
    int i = 0;
    for_values<2, 4, 8, 11, 12>([&]<auto fan_in>() {
        for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
            gates[i++] = new ClassicBranchTrainingNor<fan_in, fan_out>();
        });
    });
    gates[i++] = new ClassicBranchTrainingNor<3, 1>;
    for_values<2, 4, 8, 11, 12>([&]<auto fan_in>() {
        for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
            gates[i++] = new ClassicBranchTrainingNand<fan_in, fan_out>();
        });
    });
    for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
        gates[i++] = new ClassicBranchTrainingNot<fan_out>();
    });

    for_values<2, 4, 8, 11, 12>([&]<auto fan_in>() {
        for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
            gates[i++] = new ClassicBranchTrainingAnd<fan_in, fan_out>();
        });
    });
    for_values<1, 2, 4, 8, 11, 12>([&]<auto fan_out>() {
        gates[i++] = new ClassicBranchTrainingBuffer<fan_out>();
    });
}

classic_gates classic_bt_gates_to_test;

}