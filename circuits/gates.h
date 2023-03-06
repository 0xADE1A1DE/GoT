#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

#include "consts.h"
#include "util.h"

#define BIT_WIDTH 4

typedef struct {
    uintptr_t base_ptr;
    uint32_t offsets[GATE_FANNING];
    int inputs_amount;
    bool requires_branch_training;
} gate_metadata;


typedef uintptr_t (*gate_2)(uintptr_t, uintptr_t, uintptr_t);
typedef uintptr_t (*gate_3)(uintptr_t, uintptr_t, uintptr_t, uintptr_t);
typedef uintptr_t (*gate_4)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);

uintptr_t not_fan_gate(uintptr_t in, uintptr_t out, uintptr_t trash);

uintptr_t  not_fan_gate_impl(volatile uintptr_t in, volatile uintptr_t out, volatile bool wet_run, volatile uintptr_t trash);
uintptr_t nand_fan_gate(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash);

uintptr_t nor_fan_gate(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash);

uintptr_t fan_gate_impl(uintptr_t in, uintptr_t out1, uintptr_t out2, bool wet_run, uintptr_t trash);
uintptr_t fan_gate(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t trash);

uintptr_t and_fan_gate(uintptr_t input1, uintptr_t input2, uintptr_t output, uintptr_t trash);

uintptr_t and_fan_gate_impl_2(uintptr_t input1, uintptr_t input2, uintptr_t output1, uintptr_t output2, bool wet_run, uintptr_t trash);
uintptr_t and_fan_gate_2(uintptr_t input1, uintptr_t input2, uintptr_t output1, uintptr_t output2, uintptr_t trash);

uintptr_t or_fan_gate(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash);

uintptr_t branchless_fan_gate(uintptr_t input, uintptr_t out1, uintptr_t out2, uintptr_t trash);

uintptr_t branchless_and_fan_gate(gate_metadata *metadata, bool wet_run, uintptr_t trash);
uintptr_t branchless_or_fan_gate(gate_metadata *metadata, bool wet_run, uintptr_t trash);






uintptr_t nand_fan_gate_no_misp(gate_metadata *metadata, bool wet_run, uintptr_t trash);

uintptr_t or_from_nand_impl(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash);

uintptr_t xor_impl(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash);
uintptr_t xor_impl_2(uintptr_t in1, uintptr_t in2, uintptr_t out1, uintptr_t out2, uintptr_t trash);

uintptr_t half_adder_impl(volatile uintptr_t a, volatile uintptr_t b, volatile uintptr_t sum, volatile uintptr_t carry, volatile uintptr_t trash);

uintptr_t full_adder_impl(volatile uintptr_t a, volatile uintptr_t b, volatile uintptr_t c, volatile uintptr_t sum, volatile uintptr_t carry, volatile uintptr_t trash);

uintptr_t mux_impl(uintptr_t a, uintptr_t b, uintptr_t sel, uintptr_t output, uintptr_t trash);

uintptr_t dmux_impl(uintptr_t in, uintptr_t sel,
                    uintptr_t a, uintptr_t b, uintptr_t trash);

uintptr_t or_from_nor_not(volatile uintptr_t input1, volatile uintptr_t input2, volatile uintptr_t output, volatile uintptr_t trash);

uintptr_t not_majority_gate(uintptr_t in1, uintptr_t in2, uintptr_t in3, 
                            uintptr_t out, uintptr_t trash);

uintptr_t fan_gate_3(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, 
                     uintptr_t trash);

uintptr_t majority_gate(uintptr_t input1, uintptr_t input2, uintptr_t input3, uintptr_t output, uintptr_t trash);
uintptr_t NBitAdder_impl(uintptr_t *a, uintptr_t *b, uintptr_t *out, uintptr_t size, uintptr_t trash);

uintptr_t test_full_adder(uintptr_t trash);
uintptr_t test_mux(uintptr_t trash);
uintptr_t test_dmux(uintptr_t trash);
uintptr_t test_half_adder(uintptr_t trash);
uintptr_t test_or_from_nand_gate(uintptr_t trash);
uintptr_t test_xor(uintptr_t trash);
uintptr_t test_nand(uintptr_t trash);
uintptr_t test_or(uintptr_t trash);
uintptr_t test_branchless_fan(uintptr_t trash);
uintptr_t test_fan(uintptr_t trash);
uintptr_t test_not(uintptr_t trash);
uintptr_t test_NBitAdder(uintptr_t trash);
uintptr_t test_or_from_nor_not(uintptr_t trash);
uintptr_t test_not_majority(uintptr_t trash);
uintptr_t test_fan_3(uintptr_t trash);


uintptr_t test_not_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);
uintptr_t test_or_from_nand_gate_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_half_adder_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_full_adder_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_xor_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_dmux_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_full_adder_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_mux_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_nand_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);
uintptr_t test_fan_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);
uintptr_t test_nor_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);
uintptr_t test_or_from_nor_not_accuracy(uintptr_t trash, uintptr_t iteration);
uintptr_t test_and_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);
uintptr_t test_NBitAdder_accuracy(uintptr_t trash, uintptr_t iteration);

uintptr_t test_not_majority_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);

uintptr_t test_majority_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);

uintptr_t correct_gate_3(gate_3 gate, uintptr_t a, uintptr_t b, uintptr_t out, uintptr_t trash) ;





uintptr_t test_not_majority_5(uintptr_t trash);

uintptr_t test_not_2_from_8(uintptr_t trash);

uintptr_t fan_gate_5(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, uintptr_t out4, uintptr_t out5,
                     uintptr_t trash);

uintptr_t  not_majority_5_impl(uintptr_t in1, uintptr_t in2, uintptr_t in3, 
                               uintptr_t in4, uintptr_t in5, uintptr_t out, bool wet_run, uintptr_t trash);
uintptr_t majority_gate_5(uintptr_t input1, uintptr_t input2, uintptr_t input3, uintptr_t input4, uintptr_t input5, uintptr_t output, uintptr_t trash);


uintptr_t correct_gate_3_5(gate_3 gate, uintptr_t a, uintptr_t b, uintptr_t out, uintptr_t trash) ;

uintptr_t test_and_2_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state);

uintptr_t test_gol(uintptr_t trash);
uintptr_t test_gol_accuracy(uintptr_t trash, uintptr_t iteration);

void half_adder_pad();




uintptr_t gol_game(uintptr_t initial_state[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION],
                   uintptr_t iteration, uintptr_t trash);

uintptr_t gol_gate_impl(uintptr_t neighbours[9], 
                        uintptr_t next_state, uintptr_t trash);

uintptr_t fan_gate_impl_4(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, uintptr_t out4,
                          bool wet_run, uintptr_t trash);
uintptr_t fan_gate_4(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, uintptr_t out4, uintptr_t trash);


uintptr_t alu_impl(uintptr_t x[BIT_WIDTH], uintptr_t y[BIT_WIDTH], uintptr_t zx,
                   uintptr_t nx, uintptr_t zy, uintptr_t ny, uintptr_t f, uintptr_t no,
                   uintptr_t out[BIT_WIDTH], uintptr_t trash);

uintptr_t test_alu(uintptr_t trash);

uintptr_t gol_game_acccuracy_test(uintptr_t trash, uintptr_t iteration);

uintptr_t test_alu_accuracy(uintptr_t trash, uintptr_t iteration);

uintptr_t test_alu_accuracy_majority(uintptr_t trash, uintptr_t iteration);

uintptr_t gol_game_acccuracy_test_majority(uintptr_t trash, uintptr_t iteration);

uintptr_t gol_game_glider(uintptr_t trash, uintptr_t iteration, uintptr_t game_iteration);

uintptr_t _32BitAdder_impl(uintptr_t *a, uintptr_t *b, uintptr_t *out, uintptr_t size, uintptr_t trash);

uintptr_t test_32BitAdder_accuracy(uintptr_t trash, uintptr_t iteration);

uintptr_t sha1(uintptr_t trash, uintptr_t iteration);