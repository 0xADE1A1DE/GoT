#pragma once
#include "amplification/amplification.h"
#include "gates/gates.h"
#include "consts.h"
#include "util.h"

namespace amplification {

template<class GATE_TYPE, int ROUNDS, int STRENGTH, int DOUBLE_CACHE_LINE>
struct SimpleChainAmplificationParamaters {
    static constexpr int rounds = ROUNDS;
    static constexpr int strength = STRENGTH;

    static constexpr GATE_TYPE gate;
    static_assert(gate.m_fan_in == 1);
    static constexpr int width = gate.m_fan_out;

    static constexpr int double_cache_line_offset = DOUBLE_CACHE_LINE;
    static constexpr size_t allocation_size = LLC_CACHE_SIZE * 100;
};

template<class paramaters>
class SimpleChainAmplification : public Amplification {
public:
    // TODO: thresholds.
    SimpleChainAmplification() { }

    void initialize(uint64_t &trash) final;
    void amplify(uintptr_t target, uint64_t &trash) final;
    bool is_in_cache(uint64_t time_difference, measure::measure_type type) final;

private:
    static uintptr_t s_amplification_memory;
    uintptr_t access_pattern(int index);
};

extern Amplification &sca_100us;

}