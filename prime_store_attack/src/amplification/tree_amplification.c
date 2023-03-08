#include <assert.h>
#include "amplification/tree_amplification.h"
#include "amplification/tree_amplification_access_pattern.h"
#include "util.h"
#include "consts.h"

// With this, only one instance of tree amplification could be initialized. Consider putting this into an instance.
void *amplification_memory[MAX_META_TREE_DEPTH];
gate_bank __attribute__ ((aligned (DOUBLE_CACHE_LINE_SIZE))) g_amplification_bank = {0};

uintptr_t __always_inline initialize_amplification(amplification_metadata *amplification, uintptr_t trash, const tree_access_pattern_metadata tree_access_pattern) {
    const int amplification_layer_size = PAGE_SIZE * tree_access_pattern.total_tree_nodes * (tree_access_pattern.trees_in_allocation / (2 * tree_access_pattern.double_cache_lines_per_tree));

    assert(tree_access_pattern.meta_tree_depth <= MAX_META_TREE_DEPTH);
    for (int i = 0; i < tree_access_pattern.meta_tree_depth; i++) {
        initialize_allocation(&(amplification_memory[i]), amplification_layer_size);
    }

    for (int i = 0; i < tree_access_pattern.double_cache_lines_per_tree * (tree_access_pattern.meta_tree_depth + 1); i++) {
        amplification->affected_double_cache_lines[(i + tree_access_pattern.initial_double_cache_line) % 32] = true;
    }

    initialize_clear_cache_allocation();
    return clear_all_caches(trash);
}

uintptr_t __always_inline amplify(uintptr_t tree_root, int tree_index, int meta_depth, uintptr_t trash, const tree_access_pattern_metadata tree_access_pattern) {
    for (int depth = 0; depth < tree_access_pattern.tree_depth; depth++) {
        int layer_length = tree_access_pattern.cache_lines_per_layer(depth);
        for (int layer_index = 0; layer_index < layer_length; layer_index++) {
            g_amplification_bank.input_base = (uintptr_t)amplification_memory[meta_depth];
            g_amplification_bank.output_base = (uintptr_t)amplification_memory[meta_depth];

            for (int i = 0; i < tree_access_pattern.width; i++) {
                g_amplification_bank.output_offsets[i] = tree_access_pattern.access_pattern(tree_index, depth + 1, meta_depth, tree_access_pattern.width * layer_index + i);
            }

            if (depth != 0)
                g_amplification_bank.input_offsets[0] = tree_access_pattern.access_pattern(tree_index, depth, meta_depth, layer_index);
            else {
                g_amplification_bank.input_base = tree_root;
                g_amplification_bank.input_offsets[0] = 0;
            }
            
            trash = apply_gate(tree_access_pattern.amplification_gate, &g_amplification_bank, trash);
        }
    }
    return trash;
}

int leaf_to_tree_index(int leaf_index, int meta_depth, const tree_access_pattern_metadata tree_access_pattern) {
    if (meta_depth >= tree_access_pattern.meta_tree_depth - 1) 
        return leaf_index % tree_access_pattern.trees_in_allocation;
    return (2 * leaf_index) % tree_access_pattern.trees_in_allocation;
}

typedef uintptr_t (*meta_amplify_func_type)(uintptr_t speculatee, int leaf_index, int meta_depth, int leafs_to_recurse, uintptr_t trash);

uintptr_t __always_inline meta_amplify(uintptr_t speculatee, int leaf_index, int meta_depth, int leafs_to_recurse, uintptr_t trash, const tree_access_pattern_metadata tree_access_pattern, meta_amplify_func_type meta_amplify_func) {
    const int last_layer_cache_lines = tree_access_pattern.cache_lines_per_layer(tree_access_pattern.tree_depth);

    if (meta_depth == tree_access_pattern.meta_tree_depth) {
        trash = FORCE_READ(speculatee, trash);
        return trash;
    }

    int base_child_index = (leaf_index * leafs_to_recurse) % tree_access_pattern.trees_in_allocation;
    int tree_index = leaf_to_tree_index(leaf_index, meta_depth, tree_access_pattern);
    trash = tree_access_pattern.prefetch(speculatee, tree_index, meta_depth, trash);
    trash = amplify(speculatee, tree_index, meta_depth, trash, tree_access_pattern);

    for (int spec = 0; spec < last_layer_cache_lines; spec++) {
        int current_leaf_index = (base_child_index + spec) % last_layer_cache_lines;
        int current_base_index = (base_child_index + spec) % tree_access_pattern.trees_in_allocation;
        uintptr_t current_leaf = (uintptr_t)(amplification_memory[meta_depth]) + tree_access_pattern.access_pattern(tree_index, tree_access_pattern.tree_depth, meta_depth, current_leaf_index);

        if (spec < leafs_to_recurse)
            trash = meta_amplify_func(current_leaf, current_base_index, meta_depth + 1, last_layer_cache_lines, trash);
        else
            TEMPORAL_ADD(trash, *(uintptr_t *)current_leaf);
    }
    return trash;
}

bool tree_amplification_verify(amplification_metadata *metadata, const tree_access_pattern_metadata ap, int amplification_multiplier) {
    if (amplification_multiplier > int_pow(ap.width, ap.tree_depth) || amplification_multiplier <= 0) {
        printf("%s: amplification multiplier must be between 1 and width**tree_depth.\n", __PRETTY_FUNCTION__);
        return false;
    }
    if (ap.width != ap.amplification_gate->outputs_amount) {
        printf("%s: Mismatch between the width paramater and that gate's width.\n", __PRETTY_FUNCTION__);
        return false;
    }

    for (int meta_depth = 0; meta_depth < ap.meta_tree_depth; meta_depth++) {
        for (int tree_index = 0; tree_index < ap.trees_in_allocation; tree_index++) {
            for (int depth = 0; depth < ap.tree_depth; depth++) {
                int layer_length = ap.cache_lines_per_layer(depth);
                for (int layer_index = 0; layer_index < layer_length; layer_index++) {
                    int double_cache_line = (ap.access_pattern(tree_index, depth, meta_depth, layer_index) & (PAGE_SIZE - 1)) / DOUBLE_CACHE_LINE_SIZE;
                    if (!metadata->affected_double_cache_lines[double_cache_line]) {
                        printf("%s: Found access pattern input which invalidates our affected_double_cache_lines assumption!\n", __PRETTY_FUNCTION__);
                        printf("Paramaters were: meta depth %d, tree_index %d, depth %d, layer_index %d\n", meta_depth, tree_index, depth, layer_index);
                        printf("The double cache line accessed was: %d\n", double_cache_line);
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

#define AMPLIFICATION_MULTIPLIER_100US (8)

uintptr_t td3md2width8_100us_initialize_amplification(amplification_metadata *amplification, uintptr_t trash) {
    return initialize_amplification(amplification, trash, td3md2width8_amplification_metadata);
}

static int td3md2width8_100us_counter = 0;

uintptr_t td3md2width8_100us_meta_amplify(uintptr_t speculatee, int leaf_index, int meta_depth, int leafs_to_recurse, uintptr_t trash) {
    return meta_amplify(speculatee, leaf_index, meta_depth, leafs_to_recurse, trash, td3md2width8_amplification_metadata, td3md2width8_100us_meta_amplify);
}

uintptr_t td3md2width8_100us_amplify(uintptr_t target, uintptr_t trash) {
    return td3md2width8_100us_meta_amplify(target, td3md2width8_100us_counter++, 0, AMPLIFICATION_MULTIPLIER_100US, trash);
}

bool td3md2width8_100us_verify(amplification_metadata *metadata) {
    return tree_amplification_verify(metadata, td3md2width8_amplification_metadata, AMPLIFICATION_MULTIPLIER_100US);
}

amplification_metadata tree_amplification_100us = {
    .initialize = td3md2width8_100us_initialize_amplification,
    .amplify = td3md2width8_100us_amplify,
    .verify = td3md2width8_100us_verify
};


#define AMPLIFICATION_MULTIPLIER_1MS (64)

uintptr_t td3md2width8_1ms_initialize_amplification(amplification_metadata *amplification, uintptr_t trash) {
    return initialize_amplification(amplification, trash, td3md2width8_amplification_metadata);
}

static int td3md2width8_1ms_counter = 0;

uintptr_t td3md2width8_1ms_meta_amplify(uintptr_t speculatee, int leaf_index, int meta_depth, int leafs_to_recurse, uintptr_t trash) {
    return meta_amplify(speculatee, leaf_index, meta_depth, leafs_to_recurse, trash, td3md2width8_amplification_metadata, td3md2width8_1ms_meta_amplify);
}

uintptr_t td3md2width8_1ms_amplify(uintptr_t target, uintptr_t trash) {
    return td3md2width8_1ms_meta_amplify(target, td3md2width8_1ms_counter++, 0, AMPLIFICATION_MULTIPLIER_1MS, trash);
}

bool td3md2width8_1ms_verify(amplification_metadata *metadata) {
    return tree_amplification_verify(metadata, td3md2width8_amplification_metadata, AMPLIFICATION_MULTIPLIER_1MS);
}

amplification_metadata tree_amplification_1ms = {
    .initialize = td3md2width8_1ms_initialize_amplification,
    .amplify = td3md2width8_1ms_amplify,
    .verify = td3md2width8_1ms_verify
};