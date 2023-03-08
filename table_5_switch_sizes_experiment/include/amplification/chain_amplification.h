#pragma once
#include "amplification/amplification.h"
#include "gates/gates.h"

namespace amplification {

template<class GATE_TYPE, int TREE_DEPTH, int ROUNDS, int STRENGTH, int INITIAL_DOUBLE_CACHE_LINE, int DOUBLE_CACHE_LINES_PER_TREE, int TREES_IN_ALLOCATION>
struct ChainAmplificationParamaters {
    // TODO: static assert that GATE_TYPE is buffer, would require some more c++ magic.

    // Calculates `sum(width**i) for i in 1..tree_depth`, which is also `(width ** (tree_depth+1) - width) / (width - 1) `
    static constexpr int get_total_tree_nodes() {
        int result = 1;
        for (int i = 0; i <= tree_depth; i++)
            result *= width;

        result -= width;
        return result / (width - 1);
    }

    static constexpr GATE_TYPE gate;
    static_assert(gate.m_fan_in == 1);
    static constexpr int width = gate.m_fan_out;

    static constexpr int tree_depth = TREE_DEPTH;
    static constexpr int rounds = ROUNDS;
    static constexpr int strength = STRENGTH;

    static_assert(rounds % 2 == 0);

    static constexpr int initial_double_cache_line = INITIAL_DOUBLE_CACHE_LINE;
    static constexpr int double_cache_lines_per_tree = DOUBLE_CACHE_LINES_PER_TREE;
    static constexpr int total_tree_nodes = get_total_tree_nodes();
    static constexpr int trees_in_allocation = TREES_IN_ALLOCATION; // TODO: comment suggesting how this is picked.
};

template<class paramaters>
class ChainAmplification : public Amplification {
public:
    // TODO: thresholds.
    ChainAmplification(Amplification &sub_amplification)
        : m_sub_amplification(sub_amplification) { }

    void initialize(uint64_t &trash) final;
    void amplify(uintptr_t target, uint64_t &trash) final;
    bool is_in_cache(uint64_t time_difference, measure::measure_type type) final;

private:
    static uintptr_t s_amplification_memory;

    int depth_from_sequential_tree_index(int index);
    int get_layer_start(int depth);
    int cache_lines_per_layer(int depth);

    void prefetch(uintptr_t speculatee, int tree_index, uint64_t &trash);
    uintptr_t access_pattern(int tree_index, int sequential_tree_index);
    uintptr_t access_pattern(int tree_index, int depth, int layer_index);

    void tree_amplify(uintptr_t tree_root, int tree_index, uint64_t &trash);
    void chain_amplify(int from_tree_index, int to_tree_index, bool first_round, uint64_t &trash);

    Amplification &m_sub_amplification;
};

extern Amplification &ca_100us;

}