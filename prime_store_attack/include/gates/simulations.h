#pragma once
#include "gates.h"

void fan_simulation(gate_metadata *metadata, gate_results *results);
void and_fan_simulation(gate_metadata *metadata, gate_results *results);
void or_fan_simulation(gate_metadata *metadata, gate_results *results);

void not_fan_simulation(gate_metadata *metadata, gate_results *results);
void nand_fan_simulation(gate_metadata *metadata, gate_results *results);
void nor_fan_simulation(gate_metadata *metadata, gate_results *results);