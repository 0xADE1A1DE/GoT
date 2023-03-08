#pragma once
#include "amplification/amplification.h"
#include "gates/gates.h"

namespace amplification {

#define MAX_META_TREE_DEPTH (6)

template<class GATE_TYPE, int META_TREE_DEPTH, int TREE_DEPTH, int AMPLIFICATION_MULTIPLIER, int INITIAL_DOUBLE_CACHE_LINE, int DOUBLE_CACHE_LINES_PER_TREE, int TREES_IN_ALLOCATION>
struct TreeAmplificationParameters {
    // Calculates `sum(width**i) for i in 1..tree_depth`, which equals `(width ** (tree_depth+1) - width) / (width - 1) `
    static constexpr int get_total_tree_nodes() {
        int result = 1;
        for (int i = 0; i <= tree_depth; i++)
            result *= width;

        result -= width;
        return result / (width - 1);
    }

    static constexpr int get_threshold_rdtsc() {
        return 0;
    }

    static constexpr int get_threshold_100us() {
        return 0;
    }

    static constexpr GATE_TYPE gate;
    static_assert(gate.m_fan_in == 1);
    static constexpr int width = gate.m_fan_out;

    static constexpr int meta_tree_depth = META_TREE_DEPTH;
    static constexpr int tree_depth = TREE_DEPTH;
    static constexpr int amplification_multiplier = AMPLIFICATION_MULTIPLIER;

    static constexpr int initial_double_cache_line = INITIAL_DOUBLE_CACHE_LINE;
    static constexpr int double_cache_lines_per_tree = DOUBLE_CACHE_LINES_PER_TREE;
    static constexpr int total_tree_nodes = get_total_tree_nodes();
    static constexpr int trees_in_allocation = TREES_IN_ALLOCATION; // TODO: comment suggesting how this is picked.

    static constexpr int threshold_rdtsc = get_threshold_rdtsc();
    static constexpr int threshold_100us = get_threshold_100us();
};

template<class parameters>
class TreeAmplification : public Amplification {
public:
    TreeAmplification() { }

    void initialize(uint64_t &trash) final;
    void amplify(uintptr_t target, uint64_t &trash) final;
    bool is_in_cache(uint64_t time_difference, measure::measure_type type) final;

private:
    static uintptr_t s_amplification_memory[MAX_META_TREE_DEPTH];
    static bool s_initalized;

    static int get_layer_start(int depth);
    static int cache_lines_per_layer(int depth);

    static void prefetch(uintptr_t speculatee, int tree_index, int meta_depth, uint64_t &trash);
    static uintptr_t get_prefetch_speculatee_address(uintptr_t addr);
    static uintptr_t access_pattern(int tree_index, int depth, int meta_depth, int layer_index);

    static int leaf_to_tree_index(int leaf_index, int meta_depth);
    static void tree_amplify(uintptr_t tree_root, int tree_index, int meta_depth, uint64_t &trash);
    static void meta_amplify(uintptr_t speculatee, int leaf_index, int meta_depth, int leafs_to_recurse, uint64_t &trash);
};

extern Amplification &ta_100us;
extern Amplification &ta_1ms;
extern Amplification &ta_100ms;

}