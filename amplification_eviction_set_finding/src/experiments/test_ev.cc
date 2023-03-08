#include "amplification/tree_amplification.h"
#include "consts.h"
#include "ev/ev.h"
#include "measure/measure.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_NUMBER_OF_EV (100)

void parse_ev_cmdline(int argc, char *argv[], int *number_of_ev_to_find, int *retries_per_ev);

int test_ev(int argc, char *argv[]) {
    srand(time(0));

    int number_of_ev_to_find = DEFAULT_NUMBER_OF_EV;
    int retries_per_ev = 1;
    parse_ev_cmdline(argc, argv, &number_of_ev_to_find, &retries_per_ev);

    uintptr_t allocation = 0;

    initialize_allocation((void **)&allocation, number_of_ev_to_find * PAGE_SIZE_);

    uint64_t trash = 0;
    amplification::ta_100us.initialize(trash);

    #ifdef WASM
    for (int i = 0; i < 1000; i++) {
        amplification::ta_100us.amplify(allocation + 7 * 64, trash);
    }
    printf("This makes sure Turbofan has compiled the amplification code! %llu\n", trash);
    #endif

    ev::EvictionSetManager ev_finder(amplification::ta_100us, measure::t_100US);
    for (int i = 0; i < number_of_ev_to_find; i++) {
        ev::EvictionSet *ev = ev_finder.reduce_eviction_set(allocation + i * PAGE_SIZE_ + 7 * 64, retries_per_ev);
        if (!ev) {
            printf(RED "Failed %d :(\n" RST, i);
            continue;
        }
        printf("Success %d!\n", i);
        for (int i = 0; i < LLC_CACHE_ASSOCIATIVITY; i++) {
            printf("0x%lx\t", ev->arr[i]);
            print_cache_bucket(ev->arr[i]);
        }
    }
    return 0;
}

void parse_ev_cmdline(int argc, char *argv[], int *number_of_ev_to_find, int *retries_per_ev) {
    int opt;
    while ((opt = getopt(argc, argv, "c:r:h")) != -1) {
        switch (opt) {
        case 'c':
            *number_of_ev_to_find = atoi(optarg);
            break;
        case 'r':
            *retries_per_ev = atoi(optarg);
            break;
        case 'h':
            printf("Usage: %s %s [-c] [amount]\n", argv[0], argv[1]);
        default:
            exit(EXIT_FAILURE);
        }
    }
}
