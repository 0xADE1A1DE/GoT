#include "amplification/tree_amplification.h"
#include "consts.h"
#include "gates/classic_bt_gates.h"
#include "gates/counter_bt_gates.h"
#include <assert.h>

namespace amplification {

using AmplificationGate = gates::ClassicBranchTrainingNot<8>;
using AmplificationParameters100us = TreeAmplificationParameters<AmplificationGate, 2, 3, 24, 8, 8, 496>;
using AmplificationParameters1ms = TreeAmplificationParameters<AmplificationGate, 2, 3, 192, 8, 8, 496>;
using AmplificationParameters100ms = TreeAmplificationParameters<AmplificationGate, 3, 3, 16, 8, 8, 496>;

// NOTE: Thresholds are not defined for 1ms and 100ms amplifications, as such is_in_cache would always return false.
#ifndef WASM
template<>
constexpr int AmplificationParameters100us::get_threshold_rdtsc() {
    return 1601764; // average of averages.
}
template<>
constexpr int AmplificationParameters100us::get_threshold_100us() {
    return 7;
}

#else // #ifdef WASM
template<>
constexpr int AmplificationParameters100us::get_threshold_rdtsc() {
    // FIXME: incorrect.
    return 3196766; // average of averages.
}
template<>
constexpr int AmplificationParameters100us::get_threshold_100us() {
    return 13;
}

#endif

TreeAmplification<AmplificationParameters100us> _ta_100us;
TreeAmplification<AmplificationParameters1ms> _ta_1ms;
TreeAmplification<AmplificationParameters100ms> _ta_100ms;

Amplification &ta_100us = _ta_100us;
Amplification &ta_1ms = _ta_1ms;
Amplification &ta_100ms = _ta_100ms;

template<class parameters>
uintptr_t TreeAmplification<parameters>::s_amplification_memory[MAX_META_TREE_DEPTH];

template<class parameters>
bool TreeAmplification<parameters>::s_initalized = false;

template<class parameters>
void TreeAmplification<parameters>::initialize(uint64_t &trash) {
    if (s_initalized)
        return;
    
    const int amplification_layer_size = PAGE_SIZE_ * parameters::total_tree_nodes * (parameters::trees_in_allocation / (2 * parameters::double_cache_lines_per_tree));

    assert(parameters::meta_tree_depth <= MAX_META_TREE_DEPTH);
    for (int i = 0; i < parameters::meta_tree_depth; i++) {
        initialize_allocation((void **)&(s_amplification_memory[i]), amplification_layer_size);
    }

    initialize_clear_cache_allocation();
    clear_all_caches(trash);
    s_initalized = true;
}

template<class parameters>
void TreeAmplification<parameters>::amplify(uintptr_t target, uint64_t &trash) {
    static int counter = 0;
    return meta_amplify(target, counter++, 0, parameters::amplification_multiplier, trash);
}

template<class parameters>
bool TreeAmplification<parameters>::is_in_cache(uint64_t time_difference, measure::measure_type type) {
    switch (type) {
    case measure::t_RDTSC: return time_difference <= parameters::threshold_rdtsc;
    case measure::t_100US: return time_difference <= parameters::threshold_100us;
    default: return false;
    }
}

template<class parameters>
int TreeAmplification<parameters>::get_layer_start(int depth) {
    switch (depth) {
    case 1: return 0;
    case 2: return 8;
    case 3: return 72;
    default: return 0;
    }
}

template<class parameters>
int TreeAmplification<parameters>::cache_lines_per_layer(int depth) {
    switch (depth) {
    case 0: return 1;
    case 1: return 8;
    case 2: return 64;
    case 3: return 512;
    default: return 0;
    }
}

template<class parameters>
uintptr_t TreeAmplification<parameters>::get_prefetch_speculatee_address(uintptr_t addr) {
    // Due to how the access pattern works, and speculatee's only belonging to even tree indices,
    // adding tree_access_pattern.double_cache_lines_per_tree would bring us to the same access parameters with the odd tree index.
    uintptr_t page_offset = addr % PAGE_SIZE_;
    return (addr - page_offset) + (page_offset + parameters::double_cache_lines_per_tree * DOUBLE_CACHE_LINE_SIZE) % PAGE_SIZE_;
}

template<class parameters>
void TreeAmplification<parameters>::prefetch(uintptr_t speculatee, int tree_index, int meta_depth, uint64_t &trash) {
    if (meta_depth >= parameters::meta_tree_depth - 1)
        goto skip_subtree;

    for (int depth = 1; depth <= parameters::tree_depth; depth++) {
        for (int layer_index = 0; layer_index < cache_lines_per_layer(depth); layer_index++) {
            uintptr_t to_deref = s_amplification_memory[meta_depth] + access_pattern(tree_index + 1, depth, meta_depth, layer_index);
            TEMPORAL_ADD(trash, *(uintptr_t *)(to_deref));
        }
    }

skip_subtree:
    // Prefetching the original amplified address is up to application!
    if (meta_depth != 0)
        trash = FORCE_READ(get_prefetch_speculatee_address(speculatee), trash);
}

template<class parameters>
uintptr_t TreeAmplification<parameters>::access_pattern(int tree_index, int depth, int meta_depth, int layer_index) {
    int page_part = meta_depth;
    bool flip_layers = tree_index % 2;

    // We want to make sure the last layer ends on the first page_part for the even trees.
    page_part += (flip_layers + depth + (parameters::tree_depth % 2)) % 2;

    int layer_start = get_layer_start(depth);
    int tree_chunk_index = (tree_index / 2) % parameters::double_cache_lines_per_tree;
    int sequential_tree_index = layer_start + layer_index;

    // calc which chunk to use.
    uintptr_t result = (tree_index / (2 * parameters::double_cache_lines_per_tree)) * parameters::total_tree_nodes * PAGE_SIZE_;

    // calc in chunk offset.
    result += CACHE_LINE_SIZE;
    result += sequential_tree_index * PAGE_SIZE_;
    result += (((sequential_tree_index / parameters::width) + tree_chunk_index) % parameters::double_cache_lines_per_tree) * DOUBLE_CACHE_LINE_SIZE;

    // layers page_part component.
    result += (((page_part * parameters::double_cache_lines_per_tree) + parameters::initial_double_cache_line) * DOUBLE_CACHE_LINE_SIZE) % PAGE_SIZE_;
    return result;
}

template<class parameters>
void TreeAmplification<parameters>::tree_amplify(uintptr_t tree_root, int tree_index, int meta_depth, uint64_t &trash) {
    static gates::GateBank __attribute__((aligned(128))) bank;
    for (int depth = 0; depth < parameters::tree_depth; depth++) {
        int layer_length = cache_lines_per_layer(depth);
        for (int layer_index = 0; layer_index < layer_length; layer_index++) {

            for (int i = 0; i < parameters::width; i++) {
                bank.outputs[i] = s_amplification_memory[meta_depth] + access_pattern(tree_index, depth + 1, meta_depth, parameters::width * layer_index + i);
            }

            if (depth != 0)
                bank.inputs[0] = s_amplification_memory[meta_depth] + access_pattern(tree_index, depth, meta_depth, layer_index);
            else
                bank.inputs[0] = tree_root;

            trash = parameters::gate.apply(&bank, trash);
        }
    }
}

template<class parameters>
int TreeAmplification<parameters>::leaf_to_tree_index(int leaf_index, int meta_depth) {
    if (meta_depth >= parameters::meta_tree_depth - 1)
        return leaf_index % parameters::trees_in_allocation;
    return (2 * leaf_index) % parameters::trees_in_allocation;
}

template<class parameters>
void TreeAmplification<parameters>::meta_amplify(uintptr_t speculatee, int leaf_index, int meta_depth, int leafs_to_recurse, uint64_t &trash) {
    const int last_layer_cache_lines = cache_lines_per_layer(parameters::tree_depth);

    if (meta_depth == parameters::meta_tree_depth) {
        trash = FORCE_READ(speculatee, trash);
        return;
    }

    int base_child_index = (leaf_index * leafs_to_recurse) % parameters::trees_in_allocation;
    int tree_index = leaf_to_tree_index(leaf_index, meta_depth);
    prefetch(speculatee, tree_index, meta_depth, trash);
    tree_amplify(speculatee, tree_index, meta_depth, trash);

    for (int spec = 0; spec < last_layer_cache_lines; spec++) {
        int current_leaf_index = (base_child_index + spec) % last_layer_cache_lines;
        int current_base_index = (base_child_index + spec) % parameters::trees_in_allocation;
        uintptr_t current_leaf = s_amplification_memory[meta_depth] + access_pattern(tree_index, parameters::tree_depth, meta_depth, current_leaf_index);

        if (spec < leafs_to_recurse)
            meta_amplify(current_leaf, current_base_index, meta_depth + 1, last_layer_cache_lines, trash);
        else
            TEMPORAL_ADD(trash, *(uintptr_t *)current_leaf);
    }
}

}