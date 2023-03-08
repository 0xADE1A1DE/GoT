#include "amplification/chain_amplification.h"
#include "amplification/tree_amplification.h"
#include "consts.h"
#include "gates/classic_bt_gates.h"
#include "gates/counter_bt_gates.h"
#include <cassert>

namespace amplification {

ChainAmplification<ChainAmplificationParamaters<gates::ClassicBranchTrainingBuffer<8>, 2, 100, 3, 8, 8, 4096>> _ca_100us(ta_small);

Amplification &ca_100us = _ca_100us;

template<class paramaters>
uintptr_t ChainAmplification<paramaters>::s_amplification_memory;

template<class paramaters>
void ChainAmplification<paramaters>::initialize(uint64_t &trash) {
    const int amplification_layer_size = PAGE_SIZE_ * paramaters::total_tree_nodes * (paramaters::trees_in_allocation / (2 * paramaters::double_cache_lines_per_tree));

    m_sub_amplification.initialize(trash);

    initialize_allocation((void **)&s_amplification_memory, amplification_layer_size);

    initialize_clear_cache_allocation();
    clear_all_caches(trash);
}

template<class paramaters>
void ChainAmplification<paramaters>::amplify(uintptr_t target, uint64_t &trash) {
    static int clear_index = 0;

    int local_index = clear_index;
    int start_chain_tree_index = (local_index + 2) % paramaters::trees_in_allocation;
    int last_used_index = (start_chain_tree_index + paramaters::rounds - 1) % paramaters::trees_in_allocation;

    // This one uses both `local_index`, and the one after it.
    tree_amplify(target, local_index, trash);
    chain_amplify(local_index, start_chain_tree_index, true, trash);
    for (int i = 0; i < paramaters::rounds - 1; i++)
        chain_amplify((start_chain_tree_index + i) % paramaters::trees_in_allocation, (start_chain_tree_index + i + 1) % paramaters::trees_in_allocation, false, trash);

    for (int i = 0; i < paramaters::total_tree_nodes; i++)
        trash = FORCE_READ(s_amplification_memory + access_pattern(last_used_index, i), trash);

    clear_index = (last_used_index + 1) % paramaters::trees_in_allocation;
}

template<class paramaters>
bool ChainAmplification<paramaters>::is_in_cache(uint64_t time_difference, measure::measure_type type) {
    return 0;
}

template<class paramaters>
int ChainAmplification<paramaters>::depth_from_sequential_tree_index(int index) {
    if ((index >= 0) && (index < 8))
        return 1;
    if ((index >= 8) && (index < 72))
        return 2;
    if ((index >= 72) && (index < 584))
        return 3;
    return 0;
}

template<class paramaters>
int ChainAmplification<paramaters>::get_layer_start(int depth) {
    switch (depth) {
    case 1: return 0;
    case 2: return 8;
    case 3: return 72;
    default: return 0;
    }
}

template<class paramaters>
int ChainAmplification<paramaters>::cache_lines_per_layer(int depth) {
    switch (depth) {
    case 0: return 1;
    case 1: return 8;
    case 2: return 64;
    case 3: return 512;
    default: return 0;
    }
}

template<class paramaters>
void ChainAmplification<paramaters>::prefetch(uintptr_t speculatee, int tree_index, uint64_t &trash) {
    for (int depth = 1; depth <= paramaters::tree_depth; depth++) {
        for (int layer_index = 0; layer_index < cache_lines_per_layer(depth); layer_index++) {
            uintptr_t to_deref = s_amplification_memory + access_pattern((tree_index + 1) % paramaters::trees_in_allocation, depth, layer_index);
            TEMPORAL_ADD(trash, *(uintptr_t *)(to_deref));
        }
    }
}

template<class paramaters>
uintptr_t ChainAmplification<paramaters>::access_pattern(int tree_index, int sequential_tree_index) {
    int page_part = 0;
    bool flip_layers = tree_index % 2;

    // We want to make sure the last layer ends on the first page_part for the even trees.
    int depth = depth_from_sequential_tree_index(sequential_tree_index);
    page_part += (flip_layers + depth + (paramaters::tree_depth % 2)) % 2;

    int tree_chunk_index = (tree_index / 2) % paramaters::double_cache_lines_per_tree;

    // calc which chunk to use.
    uintptr_t result = (tree_index / (2 * paramaters::double_cache_lines_per_tree)) * paramaters::total_tree_nodes * PAGE_SIZE_;

    // calc in chunk offset.
    result += CACHE_LINE_SIZE;
    result += sequential_tree_index * PAGE_SIZE_;
    result += (((sequential_tree_index / paramaters::width) + tree_chunk_index) % paramaters::double_cache_lines_per_tree) * DOUBLE_CACHE_LINE_SIZE;

    // layers page_part component.
    result += (((page_part * paramaters::double_cache_lines_per_tree) + paramaters::initial_double_cache_line) * DOUBLE_CACHE_LINE_SIZE) % PAGE_SIZE_;
    return result;
}

template<class paramaters>
uintptr_t ChainAmplification<paramaters>::access_pattern(int tree_index, int depth, int layer_index) {
    int page_part = 0;
    bool flip_layers = tree_index % 2;

    // We want to make sure the last layer ends on the first page_part for the even trees.
    page_part += (flip_layers + depth + (paramaters::tree_depth % 2)) % 2;

    int layer_start = get_layer_start(depth);
    int tree_chunk_index = (tree_index / 2) % paramaters::double_cache_lines_per_tree;
    int sequential_tree_index = layer_start + layer_index;

    // calc which chunk to use.
    uintptr_t result = (tree_index / (2 * paramaters::double_cache_lines_per_tree)) * paramaters::total_tree_nodes * PAGE_SIZE_;

    // calc in chunk offset.
    result += CACHE_LINE_SIZE;
    result += sequential_tree_index * PAGE_SIZE_;
    result += (((sequential_tree_index / paramaters::width) + tree_chunk_index) % paramaters::double_cache_lines_per_tree) * DOUBLE_CACHE_LINE_SIZE;

    // layers page_part component.
    result += (((page_part * paramaters::double_cache_lines_per_tree) + paramaters::initial_double_cache_line) * DOUBLE_CACHE_LINE_SIZE) % PAGE_SIZE_;
    return result;
}

template<class paramaters>
void ChainAmplification<paramaters>::tree_amplify(uintptr_t tree_root, int tree_index, uint64_t &trash) {
    static gates::GateBank __attribute__((aligned(128))) bank;

    for (int depth = 0; depth < paramaters::tree_depth; depth++) {
        int layer_length = cache_lines_per_layer(depth);
        for (int layer_index = 0; layer_index < layer_length; layer_index++) {
            for (int i = 0; i < paramaters::width; i++) {
                bank.outputs[i] = s_amplification_memory + access_pattern(tree_index, depth + 1, paramaters::width * layer_index + i);
            }

            if (depth != 0)
                bank.inputs[0] = s_amplification_memory + access_pattern(tree_index, depth, layer_index);
            else
                bank.inputs[0] = tree_root;

            trash = paramaters::gate.apply(&bank, trash);
        }
    }
}

template<class paramaters>
void ChainAmplification<paramaters>::chain_amplify(int from_tree_index, int to_tree_index, bool first_round, uint64_t &trash) {
    static gates::GateBank __attribute__((aligned(128))) bank;

    int first_leaf_index = first_round ? get_layer_start(paramaters::tree_depth) : 0;
    int chain_start_index = paramaters::total_tree_nodes - paramaters::strength * (paramaters::total_tree_nodes / paramaters::width);

    for (int i = first_leaf_index; i < chain_start_index; i++) {
        // trash = FORCE_READ(s_amplification_memory + access_pattern(from_tree_index, i), trash);
        m_sub_amplification.amplify(s_amplification_memory + access_pattern(from_tree_index, i), trash);
    }

    for (int chain_index = 0; chain_index < paramaters::total_tree_nodes / paramaters::width; chain_index++) {
        for (int i = 0; i < paramaters::width; i++) {
            bank.outputs[i] = s_amplification_memory + access_pattern(to_tree_index, paramaters::width * chain_index + i);
        }

        for (int rep = 0; rep < paramaters::strength; rep++) {
            bank.inputs[0] = s_amplification_memory + access_pattern(from_tree_index, chain_start_index + chain_index * paramaters::strength + rep);
            trash = paramaters::gate.apply(&bank, trash);
        }
    }
}

}