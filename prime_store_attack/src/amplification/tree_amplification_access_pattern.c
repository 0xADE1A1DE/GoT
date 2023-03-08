#include "amplification/tree_amplification_access_pattern.h"

extern void *amplification_memory[MAX_META_TREE_DEPTH];

int get_layer_start_width8(int depth) {
    switch (depth) {
        case 1: return 0;
        case 2: return 8;
        case 3: return 72;
        default: return 0;
    }
}

int cache_lines_per_layer_width8(int depth) {
    switch (depth) {
        case 0: return 1;
        case 1: return 8;
        case 2: return 64;
        case 3: return 512;
        default: return 0;
    }
}

uintptr_t __always_inline get_prefetch_speculatee_address(uintptr_t addr, const tree_access_pattern_metadata tree_access_pattern) {
    // Due to how the access pattern works, and speculatee's only belonging to even tree indices,
    // adding tree_access_pattern.double_cache_lines_per_tree would bring us to the same access paramaters with the odd tree index.
    uintptr_t page_offset = addr % PAGE_SIZE;
    return (addr - page_offset) + (page_offset + tree_access_pattern.double_cache_lines_per_tree * DOUBLE_CACHE_LINE_SIZE) % PAGE_SIZE;
}

uintptr_t __always_inline prefetch(uintptr_t speculatee, int tree_index, int meta_depth, uintptr_t trash, const tree_access_pattern_metadata tree_access_pattern) {
    // Prefetching the original amplified address is up to application!
    if (meta_depth != 0)
        trash = FORCE_READ(get_prefetch_speculatee_address(speculatee, tree_access_pattern), trash);

    if (meta_depth >= tree_access_pattern.meta_tree_depth - 1) 
        return trash;
    
    for (int depth = 1; depth <= tree_access_pattern.tree_depth; depth++) {
        for (int layer_index = 0; layer_index < tree_access_pattern.cache_lines_per_layer(depth); layer_index++) {
            uintptr_t to_deref = (uintptr_t)amplification_memory[meta_depth] + tree_access_pattern.access_pattern(tree_index + 1, depth, meta_depth, layer_index);
            TEMPORAL_ADD(trash, *(uintptr_t *)(to_deref));
        }
    }
    return trash;
}

uintptr_t __always_inline access_pattern(int tree_index, int depth, int meta_depth, int layer_index, const tree_access_pattern_metadata tree_access_pattern) {
    int page_part = meta_depth;
    bool flip_layers = tree_index % 2;

    // We want to make sure the last layer ends on the first page_part for the even trees.
    page_part += (flip_layers + depth + (tree_access_pattern.tree_depth % 2)) % 2;
    
    int layer_start = tree_access_pattern.get_layer_start(depth);
    int tree_chunk_index = (tree_index / 2) % tree_access_pattern.double_cache_lines_per_tree;
    int sequential_tree_index = layer_start + layer_index;

    // calc which chunk to use.
    uintptr_t result = (tree_index / (2 * tree_access_pattern.double_cache_lines_per_tree)) * tree_access_pattern.total_tree_nodes * PAGE_SIZE;

    // calc in chunk offset.
    result += CACHE_LINE_SIZE;
    result += sequential_tree_index * PAGE_SIZE;
    result += (((sequential_tree_index / tree_access_pattern.width) + tree_chunk_index) % tree_access_pattern.double_cache_lines_per_tree) * DOUBLE_CACHE_LINE_SIZE;

    // layers page_part component.
    result += (((page_part * tree_access_pattern.double_cache_lines_per_tree) + tree_access_pattern.initial_double_cache_line) * DOUBLE_CACHE_LINE_SIZE) % PAGE_SIZE;
    return result;
}


uintptr_t td3md2width8_prefetch(uintptr_t speculatee, int tree_index, int meta_depth, uintptr_t trash) {
    return prefetch(speculatee, tree_index, meta_depth, trash, td3md2width8_amplification_metadata);
}

uintptr_t td3md2width8_access_pattern(int tree_index, int depth, int meta_depth, int layer_index) {
    return access_pattern(tree_index, depth, meta_depth, layer_index, td3md2width8_amplification_metadata);
}