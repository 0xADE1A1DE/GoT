#pragma once
#include "gates.h"

/* The goal of these functions is to sample inputs such that the output distribution is uniform.*/

void fan_sample_inputs(gate_metadata *metadata, gate_results *results);
void and_fan_sample_inputs(gate_metadata *metadata, gate_results *results);
void or_fan_sample_inputs(gate_metadata *metadata, gate_results *results);

void not_fan_sample_inputs(gate_metadata *metadata, gate_results *results);
void nand_fan_sample_inputs(gate_metadata *metadata, gate_results *results);
void nor_fan_sample_inputs(gate_metadata *metadata, gate_results *results);