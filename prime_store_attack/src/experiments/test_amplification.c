#include <unistd.h>
#include <math.h>
#include "amplification/tree_amplification.h"
#include "measure/measure.h"
#include "consts.h"
#include "util.h"

#define DEFAULT_NUMBER_OF_TESTS (100)

typedef struct {
    address_state input_state;
    uint64_t timing;
    bool in_cache;
} amplification_result;

void parse_amplification_cmdline(int argc, char *argv[], int *number_of_tests, bool *quiet);
void print_amplification_results(amplification_result *amplification_results, measure_units units, int number_of_tests, bool quiet);

amplification_metadata * const tested_amplification = &tree_amplification_100us;
const measure_metadata * const conforming_measure = &tree_amplification_100us_measure_metadata;
// amplification_metadata * const tested_amplification = &tree_amplification_1ms;
// const measure_metadata * const conforming_measure = &tree_amplification_1ms_measure_metadata;

int test_amplification(int argc, char *argv[]) {
    int number_of_tests = DEFAULT_NUMBER_OF_TESTS;
    bool quiet = false;
    parse_amplification_cmdline(argc, argv, &number_of_tests, &quiet);

    uintptr_t amplification_memory = 0;
    initialize_allocation((void **)&amplification_memory, PAGE_SIZE);

    amplification_result *amplification_results;
    initialize_allocation((void **)&amplification_results, sizeof(amplification_result) * number_of_tests);

    uintptr_t trash = tested_amplification->initialize(tested_amplification, trash);
    if (!tested_amplification->verify(tested_amplification))
        return 0;

    for (int i = 0; i < number_of_tests; i++) {
        address_state input_state = get_rand_range(RAM, IN_CACHE);
        address_state concrete_state;
        if (input_state == IN_CACHE)
            concrete_state = get_rand_range(L1, LLC);
        else
            concrete_state = RAM;
        
        uintptr_t to_amplify = amplification_memory + get_rand_range(1, 31) * DOUBLE_CACHE_LINE_SIZE;
        // TODO: make a util function for these, in WASM, it'll be implemented using a custom v8 native callout.
        asm volatile("mfence\nlfence\n");
        
        fetch_address(to_amplify, concrete_state);

        // TODO: make a util function for these, in WASM, it'll be implemented using a custom v8 native callout.
        asm volatile("mfence\nlfence\n");

        uint64_t result = conforming_measure->measure(to_amplify, trash);
        TEMPORAL_ADD(trash, result);

        amplification_results[i].input_state = input_state;
        amplification_results[i].timing = result;
        amplification_results[i].in_cache = conforming_measure->is_in_cache(result);
    }

    print_amplification_results(amplification_results, conforming_measure->units, number_of_tests, quiet);
}

void parse_amplification_cmdline(int argc, char *argv[], int *number_of_tests, bool *quiet) {
    int opt;
    while ((opt = getopt(argc, argv, "c:hq")) != -1) {
        switch (opt) {
        case 'c':
            *number_of_tests = atoi(optarg);
            break;
        case 'q':
            *quiet = true;
            break;
        case 'h':
            printf("Usage: %s %s [-c] [amount]\n", argv[0], argv[1]);
        default:
            exit(EXIT_FAILURE);
        }
    }
}

void print_amplification_results(amplification_result *amplification_results, measure_units units, int number_of_tests, bool quiet) {
    for (address_state state = RAM; state <= IN_CACHE; state++) {
        printf("Results when candidate is in %s\n", state_to_string(state));
        int amount = 0;
        int amount_flipped = 0;
        uint64_t sum = 0;
        for (int i = 0; i < number_of_tests; i++) {
            if (state != amplification_results[i].input_state)
                continue;
            amount++;
            amount_flipped += amplification_results[i].in_cache != (state - RAM);
            sum += amplification_results[i].timing;
            if (!quiet) {
                if (units == UNITS_RDTSC)
                    printf("Took %*ld rdtsc cycles, %f ms\n", 10, amplification_results[i].timing, rdtsc_to_ms(amplification_results[i].timing));
                else
                    printf("Took %f ms DEBUG: %ld\n", (double)amplification_results[i].timing / 10, amplification_results[i].timing);
            }
        }
        printf("In total, we had %d tests.\n", amount);
        if (!amount)
            continue;
        printf("%s rate of %f%%\n", (state == RAM) ? "0->1" : "1->0", (double)amount_flipped / amount * 100);

        uint64_t avg = sum / amount;
        uint64_t for_variance_sum = 0;
        for (int i = 0; i < number_of_tests; i++) {
            if (state != amplification_results[i].input_state)
                continue;
            for_variance_sum += int_pow(amplification_results[i].timing - avg, 2);
        }

        double std_deviation = sqrt((double)for_variance_sum / amount);
        if (units == UNITS_RDTSC)
            printf("Average time of %*ld rdtsc cycles, or %f ms, with standard deviation of %f ms\n", 10, avg, rdtsc_to_ms(avg), rdtsc_to_ms(std_deviation));
        else
            printf("Average time %f ms, with standard deviation of %f ms\n", (double)avg / 10, std_deviation / 10);
        
    }
}