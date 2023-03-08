#include "amplification/simple_chain_amplification.h"

#include "gates/classic_bt_gates.h"
#include "gates/counter_bt_gates.h"
#include <cassert>

namespace amplification {

SimpleChainAmplification<SimpleChainAmplificationParamaters<gates::ClassicBranchTrainingBuffer<12>, 10000, 3, 26>> _sca_100us;

Amplification &sca_100us = _sca_100us;

template<class paramaters>
uintptr_t SimpleChainAmplification<paramaters>::s_amplification_memory;

template<class paramaters>
void SimpleChainAmplification<paramaters>::initialize(uint64_t &trash) {
    initialize_allocation((void **)&s_amplification_memory, paramaters::allocation_size);

    initialize_clear_cache_allocation();
    clear_all_caches(trash);
}

template<class paramaters>
void SimpleChainAmplification<paramaters>::amplify(uintptr_t target, uint64_t &trash) {
    static int unused_index = 0;
    static gates::GateBank __attribute__((aligned(128))) bank;

    int local_index = unused_index;

    // Try to boost TLB resolution.
    for (int i = 0; i < paramaters::width; i++)
        trash = FORCE_READ(s_amplification_memory + access_pattern(local_index + i), trash);
    local_index += paramaters::width;

    bank.inputs[0] = target;
    for (int i = 0; i < paramaters::width; i++)
        bank.outputs[i] = s_amplification_memory + access_pattern(local_index + i);
    trash = paramaters::gate.apply(&bank, trash);

    for (int round = 1; round < paramaters::rounds; round++) {
        for (int i = 0; i < paramaters::width - paramaters::strength; i++)
            trash = FORCE_READ(s_amplification_memory + access_pattern(local_index + i), trash);

        local_index += paramaters::width;
        for (int i = 0; i < paramaters::width; i++)
            bank.outputs[i] = s_amplification_memory + access_pattern(local_index + i);

        for (int i = 0; i < paramaters::strength; i++) {
            bank.inputs[0] = s_amplification_memory + access_pattern(local_index - i - 1);
            trash = paramaters::gate.apply(&bank, trash);
        }
    }
    for (int i = 0; i < paramaters::width; i++)
        trash = FORCE_READ(s_amplification_memory + access_pattern(local_index + i), trash);
    local_index += paramaters::width;
    unused_index = local_index;
}

template<class paramaters>
bool SimpleChainAmplification<paramaters>::is_in_cache(uint64_t time_difference, measure::measure_type type) {
    return 0;
}

template<class paramaters>
uintptr_t SimpleChainAmplification<paramaters>::access_pattern(int index) {
    return (index * PAGE_SIZE_ % paramaters::allocation_size) + paramaters::double_cache_line_offset * DOUBLE_CACHE_LINE_SIZE; // + ((index * PAGE_SIZE_ / paramaters::allocation_size) % 2) * CACHE_LINE_SIZE;
}

}