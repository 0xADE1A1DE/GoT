#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "gates/gates.h"
#include "gates/bt_gates.h"
#include "gates/nbt_gates.h"
#include "measure/measure.h"

#define DEFAULT_NUMBER_OF_TESTS (4000000)

typedef struct {
    int total;
    int total_as_expected;
} gate_totals;

typedef struct {
    // With this, we acknowledge the correctness relies on our gates outputs being all the same.
    // When the output state is RAM, for simple gates at least, we know the speculative window was short.
    // For this reason we keep track of IN_CACHE and RAM.
    address_state first_simulated_output_state;
    bool as_expected;
} comparsion_result;

gate_metadata *gates_to_test[] = {
    &bt_nand_2to1_metadata,
    &bt_nand_3to1_metadata,
    &bt_nand_4to1_metadata,
    &bt_nand_5to1_metadata,
    &bt_nand_6to1_metadata,
    &bt_nand_7to1_metadata,
    &bt_nand_8to1_metadata,
    &bt_nand_9to1_metadata,
    &bt_nand_10to1_metadata,
    &bt_nand_11to1_metadata,
    &bt_nand_12to1_metadata,
    &bt_nand_13to1_metadata,
    &bt_nand_14to1_metadata,
    &bt_nor_2to1_metadata,
    &bt_nor_3to1_metadata,
    &bt_nor_4to1_metadata,
    &bt_nor_5to1_metadata,
    &bt_nor_6to1_metadata,
    &bt_nor_7to1_metadata,
    &bt_nor_8to1_metadata,
    &bt_nor_9to1_metadata,
    &bt_nor_10to1_metadata,
    &bt_nor_11to1_metadata,
    &bt_nor_12to1_metadata,
    &bt_nor_13to1_metadata,
    &bt_nor_14to1_metadata,
    &bt_not_1to1_metadata,
    &bt_not_1to2_metadata,
    &bt_not_1to3_metadata,
    &bt_not_1to4_metadata,
    &bt_not_1to5_metadata,
    &bt_not_1to6_metadata,
    &bt_not_1to7_metadata,
    &bt_not_1to8_metadata,
    &bt_not_1to9_metadata,
    &bt_not_1to10_metadata,
    &bt_not_1to11_metadata,
    &bt_not_1to12_metadata,
    &bt_not_1to13_metadata,
    &bt_not_1to14_metadata,
    &nbt_nand_2to1_metadata,
    &nbt_nand_3to1_metadata,
    &nbt_nand_4to1_metadata,
    &nbt_nand_5to1_metadata,
    &nbt_nand_6to1_metadata,
    &nbt_nand_7to1_metadata,
    &nbt_nand_8to1_metadata,
    &nbt_nand_9to1_metadata,
    &nbt_nand_10to1_metadata,
    &nbt_nand_11to1_metadata,
    &nbt_nand_12to1_metadata,
    &nbt_nand_13to1_metadata,
    &nbt_nand_14to1_metadata,
    &nbt_not_1to1_metadata,
    &nbt_not_1to2_metadata,
    &nbt_not_1to3_metadata,
    &nbt_not_1to4_metadata,
    &nbt_not_1to5_metadata,
    &nbt_not_1to6_metadata,
    &nbt_not_1to7_metadata,
    &nbt_not_1to8_metadata,
    &nbt_not_1to9_metadata,
    &nbt_not_1to10_metadata,
    &nbt_not_1to11_metadata,
    &nbt_not_1to12_metadata,
    &nbt_not_1to13_metadata,
    &nbt_not_1to14_metadata,
};

void parse_gates_cmdline(int argc, char *argv[], int *number_of_tests);
comparsion_result test_gate(gate_metadata *metadata, uintptr_t gate_io_memory);
comparsion_result compare_to_simulation(gate_metadata *metadata, gate_results *results);

int test_gates(int argc, char *argv[]) {
    // We want some randomness when choosing gates/inputs, we must init it.
    srand(time(0));

    int number_of_tests = DEFAULT_NUMBER_OF_TESTS;
    parse_gates_cmdline(argc, argv, &number_of_tests);

    uintptr_t gate_io_memory = 0;
    initialize_allocation((void **)&gate_io_memory, (GATE_MAX_INPUTS + GATE_MAX_OUTPUTS) * PAGE_SIZE);
    
    const int number_of_gates = sizeof(gates_to_test) / sizeof(gate_metadata *);
    gate_totals totals[number_of_gates][2];
    for (int gate_index = 0; gate_index < number_of_gates; gate_index++) {
        for (int i = 0; i < 2; i++) {
            totals[gate_index][i].total = 0;
            totals[gate_index][i].total_as_expected = 0;
        }
    }

    for (int i = 0; i < number_of_tests; i++) {
        int chosen_gate_index = get_rand_range(0, number_of_gates - 1);
        gate_metadata *chosen_gate = gates_to_test[chosen_gate_index];

        comparsion_result result = test_gate(chosen_gate, gate_io_memory);

        gate_totals *current_total = &totals[chosen_gate_index][result.first_simulated_output_state - RAM];
        current_total->total_as_expected += result.as_expected;
        current_total->total++;
    }

    for (int i = 0; i < number_of_gates; i++) {
        gate_metadata *current_gate = gates_to_test[i];
        printf("Results for %s\n", current_gate->gate_name);
        for (int res = 0; res < 2; res++) {
            gate_totals *current = &totals[i][res];
            printf("\tOutput should have been %s, ", state_to_string(res == 0 ? RAM : IN_CACHE));
            printf("we have %d / %d ~ %f as expected.\n", current->total_as_expected, current->total, (float)current->total_as_expected / current->total);
        }
    }
}

void parse_gates_cmdline(int argc, char *argv[], int *number_of_tests) {
    int opt;
    while ((opt = getopt(argc, argv, "c:h")) != -1) {
        switch (opt) {
        case 'c':
            *number_of_tests = atoi(optarg);
            break;
        case 'h':
            printf("Usage: %s %s [-c] [amount]\n", argv[0], argv[1]);
        default:
            exit(EXIT_FAILURE);
        }
    }
}

comparsion_result test_gate(gate_metadata *metadata, uintptr_t gate_io_memory) {
    static gate_results results = {0};
    static gate_bank bank = {0};
    bank.input_base = gate_io_memory;
    bank.output_base = gate_io_memory;

    metadata->sample_inputs_state(metadata, &results);

    for (int i = 0; i < metadata->inputs_amount; i++) {
        bank.input_offsets[i] = PAGE_SIZE * i + get_rand_range(0, 31) * DOUBLE_CACHE_LINE_SIZE;
        fetch_address(bank.input_base + bank.input_offsets[i], results.inputs[i]);
    }
    for (int i = 0; i < metadata->outputs_amount; i++) {
        bank.output_offsets[i] = PAGE_SIZE * (i + metadata->inputs_amount) + get_rand_range(0, 31) * DOUBLE_CACHE_LINE_SIZE;
        fetch_address(bank.output_base + bank.output_offsets[i], RAM);
    }

    // TODO: make a util function for these, in WASM, it'll be implemented using a custom v8 native callout.
    asm volatile("mfence\nlfence\n");

    uintptr_t trash = apply_gate(metadata, &bank, trash);

    for (int i = 0; i < metadata->outputs_amount; i++) {
        uint64_t result = rdtsc_measure_metadata.measure(bank.output_base + bank.output_offsets[i], trash);
        trash += result;
        results.outputs[i] = RAM + rdtsc_measure_metadata.is_in_cache(result);
    }
    return compare_to_simulation(metadata, &results);
}

comparsion_result compare_to_simulation(gate_metadata *metadata, gate_results *results) {
    static gate_results simulation = {0};
    for (int i = 0; i < metadata->inputs_amount; i++) {
        simulation.inputs[i] = results->inputs[i];
    }
    metadata->simulate_gate(metadata, &simulation);
    bool valid = true;
    for (int i = 0; i < metadata->outputs_amount; i++) {
        valid &= (simulation.outputs[i] == results->outputs[i]);
    }
    return (comparsion_result){simulation.outputs[0], valid};
}
