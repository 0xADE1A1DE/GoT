#pragma once
#include "gates.h"

namespace gates {

template<int fan_in, int fan_out>
class CounterBranchTrainingNand : public GateBase<GateType::NAND, false> {
public:
    constexpr CounterBranchTrainingNand<fan_in, fan_out>()
        : GateBase(fan_in, fan_out) {};

    uint64_t gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const final;

    const char *gate_type() const { return "counter branch training nand"; }
};

template<int fan_in, int fan_out>
class CounterBranchTrainingAnd : public GateBase<GateType::AND, false> {
public:
    constexpr CounterBranchTrainingAnd<fan_in, fan_out>()
        : GateBase(fan_in, fan_out) {};

    uint64_t gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const final;

    const char *gate_type() const { return "counter branch training and"; }
};

template<int fan_out>
class CounterBranchTrainingBuffer : public CounterBranchTrainingAnd<1, fan_out> {
    const char *gate_type() const final { return "counter branch training buffer"; }
};

template<int fan_out>
class CounterBranchTrainingNot : public CounterBranchTrainingNand<1, fan_out> {
    const char *gate_type() const final { return "counter branch training not"; }
};

class counter_gates {
    public:
    constexpr counter_gates();

    Gate *gates[2];
};

extern counter_gates counter_bt_gates_to_test;
constexpr int number_of_counter_bt_gates = sizeof(counter_bt_gates_to_test.gates) / sizeof(*counter_bt_gates_to_test.gates);

}