#pragma once
#include <stdint.h>
#include "gates/gates.h"
#include "gates/bt_gates.h"
#include "gates/nbt_gates.h"

#define MAX_META_TREE_DEPTH (6)

typedef int (*get_layer_start_func_type)(int depth);
typedef int (*cache_lines_per_layer_func_type)(int depth);

typedef uintptr_t (*prefetch_func_type)(uintptr_t speculatee, int tree_index, int meta_depth, uintptr_t trash);
typedef uintptr_t (*access_pattern_func_type)(int tree_index, int depth, int meta_depth, int layer_index);

typedef struct {
    int meta_tree_depth;
    int tree_depth;
    int width;
    // This should match the width paramater.
    gate_metadata *amplification_gate;

    // These are paramaters specific for our scheme.
    int initial_double_cache_line;
    int double_cache_lines_per_tree;
    int total_tree_nodes; // a precomputed `sum(width**i) for i in 1..tree_depth`
    int trees_in_allocation; // TODO: comment suggesting how this is picked.

    // These are preoptimized functions considering the width.
    get_layer_start_func_type get_layer_start;
    cache_lines_per_layer_func_type cache_lines_per_layer;

    // These shall be optimized by virtue of inlining tricks.
    prefetch_func_type prefetch;
    access_pattern_func_type access_pattern;
} tree_access_pattern_metadata;


int get_layer_start_width8(int depth);
int cache_lines_per_layer_width8(int depth);
uintptr_t td3md2width8_prefetch(uintptr_t speculatee, int tree_index, int meta_depth, uintptr_t trash);
uintptr_t td3md2width8_access_pattern(int tree_index, int depth, int meta_depth, int layer_index);

static const tree_access_pattern_metadata td3md2width8_amplification_metadata = {
    .meta_tree_depth = 2,
    .tree_depth = 3,
    .width = 8,
    .amplification_gate = &bt_not_1to8_metadata,

    .initial_double_cache_line = 8,
    .double_cache_lines_per_tree = 8,
    .total_tree_nodes = 584,
    .trees_in_allocation = 512,

    .get_layer_start = get_layer_start_width8,
    .cache_lines_per_layer = cache_lines_per_layer_width8,

    .prefetch = td3md2width8_prefetch,
    .access_pattern = td3md2width8_access_pattern
};