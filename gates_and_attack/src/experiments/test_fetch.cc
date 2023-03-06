#include "consts.h"
#include "measure/measure.h"
#include "util.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_NUMBER_OF_TESTS (20000)

void test_fetch_once(uintptr_t memory, uint64_t &trash);
void parse_fetch_cmdline(int argc, char *argv[], int *number_of_tests);

int test_fetch(int argc, char *argv[]) {
    srand(time(0));

    int number_of_tests = DEFAULT_NUMBER_OF_TESTS;
    parse_fetch_cmdline(argc, argv, &number_of_tests);

    uintptr_t memory = 0;
    uint64_t trash = 0;
    initialize_allocation((void **)&memory, PAGE_SIZE_);

    for (int i = 0; i < number_of_tests; i++) {
        test_fetch_once(memory, trash);
    }
    return 0;
}

void test_fetch_once(uintptr_t memory, uint64_t &trash) {
    address_state state_to_test = (address_state)get_rand_range(L1, RAM);

    memory_fences();

    uintptr_t test_address = memory + get_rand_range(0, 31) * DOUBLE_CACHE_LINE_SIZE;
    fetch_address(test_address + CACHE_LINE_SIZE, RAM);
    fetch_address(test_address, RAM);

    memory_fences();

    fetch_address(test_address, state_to_test);

    memory_fences();

    measure::Measure<measure::t_RDTSC> measure_input;
    uint64_t result = measure_input.measure(test_address, trash);
    printf("%4" PRId64 " %s\n", result, state_to_string(state_to_test));
}

void parse_fetch_cmdline(int argc, char *argv[], int *number_of_tests) {
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
