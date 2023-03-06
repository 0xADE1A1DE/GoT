#pragma once
#include "gates/gates.h"

namespace gates {

template<int fan_in, int fan_out>
class ClassicBranchTrainingNand : public GateBase<GateType::NAND, true> {
public:
    constexpr ClassicBranchTrainingNand<fan_in, fan_out>()
        : GateBase(fan_in, fan_out) {};

    uint64_t gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const final;

    const char *gate_type() const { return "classic branch training nand"; }
};

template<int fan_in, int fan_out>
class ClassicBranchTrainingNor : public GateBase<GateType::NOR, true> {
public:
    constexpr ClassicBranchTrainingNor<fan_in, fan_out>()
        : GateBase(fan_in, fan_out) {};

    uint64_t gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const final;

    const char *gate_type() const { return "classic branch training nor"; }
};

template<int fan_in, int fan_out>
class ClassicBranchTrainingAnd : public GateBase<GateType::AND, true> {
public:
    constexpr ClassicBranchTrainingAnd<fan_in, fan_out>()
        : GateBase(fan_in, fan_out) {};

    uint64_t gate_implementation(int wet_run, GateBank *bank, uint64_t trash) const final;

    const char *gate_type() const { return "classic branch training and"; }
};

template<int fan_out>
class ClassicBranchTrainingBuffer : public ClassicBranchTrainingAnd<1, fan_out> {
    const char *gate_type() const final { return "classic branch training buffer"; }
};

template<int fan_out>
class ClassicBranchTrainingNot : public ClassicBranchTrainingNand<1, fan_out> {
    const char *gate_type() const final { return "classic branch training not"; }
};

class classic_gates {
public:
    constexpr classic_gates();

    Gate *gates[103];
};

extern classic_gates classic_bt_gates_to_test;
constexpr int number_of_classic_bt_gates = sizeof(classic_bt_gates_to_test.gates) / sizeof(*classic_bt_gates_to_test.gates);

}