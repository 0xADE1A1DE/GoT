#include "amplification/tree_amplification.h"
#include "consts.h"
#include "measure/measure.h"
#include "util.h"
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

using namespace amplification;

#define DEFAULT_NUMBER_OF_TESTS (100)

typedef struct {
    address_state input_state;
    uint64_t timing;
    bool in_cache;
} AmplificationResult;

void parse_amplification_cmdline(int argc, char *argv[], int *number_of_tests, bool *quiet);
void print_amplification_results(AmplificationResult *amplification_results, int number_of_tests, bool quiet);

#ifndef WASM
Amplification &tested_amplification = ta_100ms;
#else
Amplification &tested_amplification = ta_1ms;
#endif

constexpr measure::measure_type amplification_measure_units = measure::t_100US;

int test_amplification(int argc, char *argv[]) {
    int number_of_tests = DEFAULT_NUMBER_OF_TESTS;
    bool quiet = false;
    parse_amplification_cmdline(argc, argv, &number_of_tests, &quiet);

    uintptr_t amplification_memory = 0;
    initialize_allocation((void **)&amplification_memory, PAGE_SIZE_);

    AmplificationResult *amplification_results;
    initialize_allocation((void **)&amplification_results, sizeof(AmplificationResult) * number_of_tests);

    uint64_t trash = 0;
    tested_amplification.initialize(trash);

    for (int i = 0; i < number_of_tests; i++) {
        address_state input_state = (address_state)(RAM + i % 2);
        address_state concrete_state = input_state;
        if (input_state == IN_CACHE)
            concrete_state = (address_state)get_rand_range(L1, LLC);

        uintptr_t target = amplification_memory + get_rand_range(1, 31) * DOUBLE_CACHE_LINE_SIZE;
        memory_fences();
        fetch_address(target, concrete_state);
        memory_fences();

        measure::AmplificationMeasure<amplification_measure_units> m(tested_amplification);

        amplification_results[i].input_state = input_state;
        amplification_results[i].timing = m.measure(target, trash);
        amplification_results[i].in_cache = m.in_cache(amplification_results[i].timing);
    }

    print_amplification_results(amplification_results, number_of_tests, quiet);
    return 0;
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

void print_amplification_results(AmplificationResult *amplification_results, int number_of_tests, bool quiet) {
    for (int state = RAM; state <= IN_CACHE; state++) {
        printf("Results when candidate is in %s\n", state_to_string((address_state)state));
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
                if (amplification_measure_units == measure::t_RDTSC)
                    printf("Took %*" PRId64 " rdtsc cycles, %f ms\n", 10, amplification_results[i].timing, rdtsc_to_ms(amplification_results[i].timing));
                else
                    printf("Took %f ms\n", (double)amplification_results[i].timing / 10);
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
        if (amplification_measure_units == measure::t_RDTSC)
            printf("Average time of %*" PRId64 " rdtsc cycles, or %f ms, with standard deviation of %f ms\n", 10, avg, rdtsc_to_ms(avg), rdtsc_to_ms(std_deviation));
        else
            printf("Average time %f ms, with standard deviation of %f ms\n", (double)avg / 10, std_deviation / 10);
    }
}