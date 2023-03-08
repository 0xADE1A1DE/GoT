#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "consts.h"
#include "util.h"

#define GATE_MAX_INPUTS (14)
#define GATE_MAX_OUTPUTS (14)

typedef struct _gate_metadata gate_metadata;

typedef struct {
    uintptr_t input_base;
    uint32_t input_offsets[GATE_MAX_INPUTS];
    uintptr_t output_base;
    uint32_t output_offsets[GATE_MAX_OUTPUTS];
} gate_bank;

typedef struct  {
    address_state inputs[GATE_MAX_INPUTS];
    address_state outputs[GATE_MAX_OUTPUTS];
} gate_results;

// TODO: consider adding a counter to the gate_func_type - could help decrease the memory footprint of nbt.
typedef uintptr_t (*gate_func_type)(int wet_run, gate_bank *bank, uintptr_t trash);
typedef void (*gate_input_state_sample_func_type)(gate_metadata *metadata, gate_results *results);
typedef void (*gate_simulation_func_type)(gate_metadata *metadata, gate_results *results);

struct _gate_metadata{
    int inputs_amount;
    int outputs_amount;
    bool requires_branch_training;
    gate_func_type gate_func;
    gate_input_state_sample_func_type sample_inputs_state;
    gate_simulation_func_type simulate_gate;
    char *gate_name;
};

uintptr_t apply_gate(gate_metadata *metadata, gate_bank *bank, uintptr_t trash);