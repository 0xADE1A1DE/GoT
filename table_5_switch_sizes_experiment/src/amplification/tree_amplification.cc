#include "amplification/tree_amplification.h"
#include "consts.h"
#include "gates/classic_bt_gates.h"
#include "gates/counter_bt_gates.h"
#include <cassert>

namespace amplification {

using AmplificationGate = gates::ClassicBranchTrainingNot<8>;

// This is used in chain amplification.
TreeAmplification<TreeAmplificationParamaters<AmplificationGate, 1, 3, 512, 8, 8, 512>> _ta_small;
Amplification &ta_small = _ta_small;

TreeAmplification<TreeAmplificationParamaters<AmplificationGate, 2, 3, 8, 8, 8, 512>> _ta_100us;
TreeAmplification<TreeAmplificationParamaters<AmplificationGate, 2, 3, 64, 8, 8, 512>> _ta_1ms;
TreeAmplification<TreeAmplificationParamaters<AmplificationGate, 2, 3, 512, 8, 8, 512>> _ta_10ms;
TreeAmplification<TreeAmplificationParamaters<AmplificationGate, 3, 3, 16, 8, 8, 512>> _ta_100ms;

Amplification &ta_100us = _ta_100us;
Amplification &ta_1ms = _ta_1ms;
Amplification &ta_10ms = _ta_10ms;
Amplification &ta_100ms = _ta_100ms;

template<class paramaters>
uintptr_t TreeAmplification<paramaters>::s_amplification_memory[MAX_META_TREE_DEPTH];

template<class paramaters>
void TreeAmplification<paramaters>::initialize(uint64_t &trash) {
    const int amplification_layer_size = PAGE_SIZE_ * paramaters::total_tree_nodes * (paramaters::trees_in_allocation / (2 * paramaters::double_cache_lines_per_tree));

    assert(paramaters::meta_tree_depth <= MAX_META_TREE_DEPTH);
    for (int i = 0; i < paramaters::meta_tree_depth; i++) {
        initialize_allocation((void **)&(s_amplification_memory[i]), amplification_layer_size);
    }

    initialize_clear_cache_allocation();
    clear_all_caches(trash);
}

template<class paramaters>
void TreeAmplification<paramaters>::amplify(uintptr_t target, uint64_t &trash) {
    static int counter = 0;
    return meta_amplify(target, counter++, 0, paramaters::amplification_multiplier, trash);
}

template<class paramaters>
bool TreeAmplification<paramaters>::is_in_cache(uint64_t time_difference, measure::measure_type type) {
    return 0;
}

template<class paramaters>
int TreeAmplification<paramaters>::get_layer_start(int depth) {
    switch (depth) {
    case 1: return 0;
    case 2: return 8;
    case 3: return 72;
    default: return 0;
    }
}

template<class paramaters>
int TreeAmplification<paramaters>::cache_lines_per_layer(int depth) {
    switch (depth) {
    case 0: return 1;
    case 1: return 8;
    case 2: return 64;
    case 3: return 512;
    default: return 0;
    }
}

template<class paramaters>
uintptr_t TreeAmplification<paramaters>::get_prefetch_speculatee_address(uintptr_t addr) {
    // Due to how the access pattern works, and speculatee's only belonging to even tree indices,
    // adding tree_access_pattern.double_cache_lines_per_tree would bring us to the same access paramaters with the odd tree index.
    uintptr_t page_offset = addr % PAGE_SIZE_;
    return (addr - page_offset) + (page_offset + paramaters::double_cache_lines_per_tree * DOUBLE_CACHE_LINE_SIZE) % PAGE_SIZE_;
}

template<class paramaters>
void TreeAmplification<paramaters>::prefetch(uintptr_t speculatee, int tree_index, int meta_depth, uint64_t &trash) {
    if (meta_depth >= paramaters::meta_tree_depth - 1)
        goto skip_subtree;

    for (int depth = 1; depth <= paramaters::tree_depth; depth++) {
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

template<class paramaters>
uintptr_t TreeAmplification<paramaters>::access_pattern(int tree_index, int depth, int meta_depth, int layer_index) {
    int page_part = meta_depth;
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
void TreeAmplification<paramaters>::tree_amplify(uintptr_t tree_root, int tree_index, int meta_depth, uint64_t &trash) {
    static gates::GateBank __attribute__((aligned(128))) bank;
    for (int depth = 0; depth < paramaters::tree_depth; depth++) {
        int layer_length = cache_lines_per_layer(depth);
        for (int layer_index = 0; layer_index < layer_length; layer_index++) {

            for (int i = 0; i < paramaters::width; i++) {
                bank.outputs[i] = s_amplification_memory[meta_depth] + access_pattern(tree_index, depth + 1, meta_depth, paramaters::width * layer_index + i);
            }

            if (depth != 0)
                bank.inputs[0] = s_amplification_memory[meta_depth] + access_pattern(tree_index, depth, meta_depth, layer_index);
            else
                bank.inputs[0] = tree_root;

            trash = paramaters::gate.apply(&bank, trash);
        }
    }
}

template<class paramaters>
int TreeAmplification<paramaters>::leaf_to_tree_index(int leaf_index, int meta_depth) {
    if (meta_depth >= paramaters::meta_tree_depth - 1)
        return leaf_index % paramaters::trees_in_allocation;
    return (2 * leaf_index) % paramaters::trees_in_allocation;
}

template<class paramaters>
void TreeAmplification<paramaters>::meta_amplify(uintptr_t speculatee, int leaf_index, int meta_depth, int leafs_to_recurse, uint64_t &trash) {
    const int last_layer_cache_lines = cache_lines_per_layer(paramaters::tree_depth);

    if (meta_depth == paramaters::meta_tree_depth) {
        trash = FORCE_READ(speculatee, trash);
        return;
    }

    int base_child_index = (leaf_index * leafs_to_recurse) % paramaters::trees_in_allocation;
    int tree_index = leaf_to_tree_index(leaf_index, meta_depth);
    prefetch(speculatee, tree_index, meta_depth, trash);
    tree_amplify(speculatee, tree_index, meta_depth, trash);

    for (int spec = 0; spec < last_layer_cache_lines; spec++) {
        int current_leaf_index = (base_child_index + spec) % last_layer_cache_lines;
        int current_base_index = (base_child_index + spec) % paramaters::trees_in_allocation;
        uintptr_t current_leaf = s_amplification_memory[meta_depth] + access_pattern(tree_index, paramaters::tree_depth, meta_depth, current_leaf_index);

        if (spec < leafs_to_recurse)
            meta_amplify(current_leaf, current_base_index, meta_depth + 1, last_layer_cache_lines, trash);
        else
            TEMPORAL_ADD(trash, *(uintptr_t *)current_leaf);
    }
}

}