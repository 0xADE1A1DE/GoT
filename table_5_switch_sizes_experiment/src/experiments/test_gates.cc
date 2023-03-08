#include "consts.h"
#include "gates/classic_bt_gates.h"
#include "gates/counter_bt_gates.h"
#include "gates/gates.h"
#include "measure/measure.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace gates;

#define DEFAULT_NUMBER_OF_TESTS (20000)

typedef struct {
    int total;
    int total_as_expected;
    int total_count;
    uint64_t total_time;
} TestGateTotals;

typedef struct {
    // With this, we acknowledge the correctness relies on our gates outputs being all the same.
    // When the output state is RAM, for simple gates at least, we know the speculative window was short.
    // For this reason we keep track of IN_CACHE and RAM.
    address_state first_simulated_output_state;
    bool as_expected;
    int count;
    uint64_t gate_time;
} ComparsionResult;

void parse_gates_cmdline(int argc, char *argv[], int *number_of_tests_per_gate);
ComparsionResult test_gate(Gate *gate, uintptr_t gate_io_memory, int test_index);
ComparsionResult compare_to_simulation(Gate *gate, GateResults &results);

int test_gates(int argc, char *argv[]) {
    // We want some randomness when choosing gates/inputs, we must init it.
    srand(time(0));

    int number_of_tests_per_gate = DEFAULT_NUMBER_OF_TESTS;
    parse_gates_cmdline(argc, argv, &number_of_tests_per_gate);

    uintptr_t gate_io_memory = 0;
    initialize_allocation((void **)&gate_io_memory, (MAX_INPUTS + MAX_OUTPUTS) * PAGE_SIZE_);

    int i = 0;
    int number_of_gates = number_of_counter_bt_gates;
    Gate *gates_to_test[number_of_gates];
    for (Gate *current : counter_bt_gates_to_test.gates)
        gates_to_test[i++] = current;

    TestGateTotals totals[number_of_gates][2];
    for (int gate_index = 0; gate_index < number_of_gates; gate_index++) {
        for (int i = 0; i < 2; i++) {
            totals[gate_index][i].total = 0;
            totals[gate_index][i].total_as_expected = 0;
            totals[gate_index][i].total_count = 0;
            totals[gate_index][i].total_time = 0;
        }
    }
    for (int gate_index = 0; gate_index < number_of_gates; gate_index++) {
        for (int i = 0; i < number_of_tests_per_gate; i++) {
            Gate *current_gate = gates_to_test[gate_index];

            ComparsionResult result = test_gate(current_gate, gate_io_memory, i);

            TestGateTotals *current_total = &totals[gate_index][result.first_simulated_output_state - RAM];
            current_total->total_count += result.count;
            current_total->total_as_expected += result.as_expected;
            current_total->total++;
            current_total->total_time += result.gate_time;
        }
    }

    for (int i = 0; i < number_of_gates; i++) {
        Gate *current_gate = gates_to_test[i];
        printf("Results for ");
        current_gate->print_gate_name();
        printf("\n");

        for (int res = 0; res < 2; res++) {
            TestGateTotals *current = &totals[i][res];
            printf("\tOutput should have been %s, ", state_to_string(res == 0 ? RAM : IN_CACHE));
            printf("we have %d / %d ~ %f as expected,", current->total_as_expected, current->total, (float)current->total_as_expected / current->total);
            printf("counter value - %d. ", current->total_count);
            printf("Took average of %f rdtsc cycles\n", (double)(current->total_time) / (current->total));
        }
    }
    return 0;
}

void parse_gates_cmdline(int argc, char *argv[], int *number_of_tests_per_gate) {
    int opt;
    while ((opt = getopt(argc, argv, "c:h")) != -1) {
        switch (opt) {
        case 'c':
            *number_of_tests_per_gate = atoi(optarg);
            break;
        case 'h':
            printf("Usage: %s %s [-c] [amount]\n", argv[0], argv[1]);
        default:
            exit(EXIT_FAILURE);
        }
    }
}

ComparsionResult test_gate(Gate *gate, uintptr_t gate_io_memory, int test_index) {
    static GateResults results = {};
    static GateBank __attribute__((aligned(128))) bank = { 0 };

    gate->sample_input_states(results);

    memory_fences();
    for (int i = 0; i < gate->m_fan_in; i++) {
        bank.inputs[i] = gate_io_memory + PAGE_SIZE_ * i + get_rand_range(0, 31) * DOUBLE_CACHE_LINE_SIZE;
        fetch_address(bank.inputs[i] + CACHE_LINE_SIZE, RAM);
        fetch_address(bank.inputs[i], RAM);
    }
    for (int i = 0; i < gate->m_fan_out; i++) {
        bank.outputs[i] = gate_io_memory + PAGE_SIZE_ * (i + gate->m_fan_in) + get_rand_range(0, 31) * DOUBLE_CACHE_LINE_SIZE;
        fetch_address(bank.outputs[i], RAM);
    }

    memory_fences();
    for (int i = 0; i < gate->m_fan_in; i++) {
        fetch_address(bank.inputs[i], results.inputs[i]);
    }
    memory_fences();

    // We need to avoid using a fixed trash value (at compile time).
    uint64_t trash = gate_io_memory;

    measure::Measure<measure::t_RDTSC> rdtsc_measure;
    uint64_t gate_time = rdtsc_measure.measure([&](uint64_t &trash_) {
        trash_ = gate->apply(&bank, trash_);
    }, trash);

    for (int i = 0; i < gate->m_fan_out; i++) {
        uint64_t result = rdtsc_measure.measure(bank.outputs[i], trash);
        results.outputs[i] = (address_state)(RAM + rdtsc_measure.in_cache(result));
    }

    ComparsionResult res = compare_to_simulation(gate, results);
    res.gate_time = gate_time;
    return res;
}

ComparsionResult compare_to_simulation(Gate *gate, GateResults &results) {
    static GateResults simulation = {};
    for (int i = 0; i < gate->m_fan_in; i++) {
        simulation.inputs[i] = results.inputs[i];
    }
    gate->simulate(simulation);
    int count = 0;
    for (int i = 0; i < gate->m_fan_out; i++) {
        count += (simulation.outputs[i] == results.outputs[i]);
    }
    return (ComparsionResult) { .first_simulated_output_state = simulation.outputs[0], .as_expected = (count == gate->m_fan_out), .count = count };
}
