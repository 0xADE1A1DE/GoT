#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include "measure/measure.h"
#include "amplification/tree_amplification.h"
#include "retention/retention.h"
#include "util.h"
#include "consts.h"

void parse_retention_cmdline(int argc, char *argv[], int *pages, int *per_ev, int *shrink_factor, int *interleave);

amplification_metadata * const retention_amplification = &tree_amplification_100us;
const measure_metadata * const retention_conforming_measure = &tree_amplification_100us_measure_metadata;

int test_retention(int argc, char *argv[]) {
    int pages = 0;
    int per_ev = 0;
    int shrink_factor = 3;
    int interleave = 3;
    parse_retention_cmdline(argc, argv, &pages, &per_ev, &shrink_factor, &interleave);
    if (pages == 0 || per_ev == 0) {
        printf("Usage: %s %s [-p] [pages] [-e] [per_ev] [-s] [shrink_factor] [-i] [interleave]\n", argv[0], argv[1]);
        return 0;
    }
    initialize_clear_cache_allocation();

    uintptr_t trash = retention_amplification->initialize(retention_amplification, trash);
    if (!retention_amplification->verify(retention_amplification))
        return 0;

    eviction_set *eviction_sets = find_eviction_sets(100);
    assert(eviction_sets != NULL);

    retention_metadata __attribute__ ((aligned (PAGE_SIZE))) metadata = {.allowed_double_cache_lines={0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, .pages=pages, .per_ev=per_ev, .shrink_factor=shrink_factor, .interleave=interleave};
    retention_scheme __attribute__ ((aligned (PAGE_SIZE))) scheme = initialize_retention_scheme(metadata, eviction_sets);

    int retention_bank_size = total_retention_entries(scheme.metadata);
    int measure_size = total_measure_entries(scheme.metadata);
    int decayed = 0;

    char *measurements = malloc(measure_size);
    char *pattern = malloc(retention_bank_size);
    for (int i = 0; i < retention_bank_size; i++)
        pattern[i] = 1;

    trash = clear_all_caches(0);

    for (int i = 0; i < retention_bank_size; i++) {
        asm volatile("mfence\nlfence");
        if (pattern[i])
            fetch_address(retention_get_entry(&scheme, i), LLC);
        asm volatile("mfence\nlfence");
    }


    for (int i = 0; i < measure_size; i++) {
        uint64_t result = retention_measure(&scheme, i, retention_conforming_measure, trash);
        trash += result;
        measurements[i] = !retention_conforming_measure->is_in_cache(result);
        decayed += (measurements[i] != pattern[i]);
    }

    printf("Decays: %d/%d\n", decayed, measure_size);
    for (int i = 0; i < measure_size; i++) {
        printf("%01x", measurements[i]);
    }
    printf("\n");
    return 0;
}

void parse_retention_cmdline(int argc, char *argv[], int *pages, int *per_ev, int *shrink_factor, int *interleave) {
    int opt;
    while ((opt = getopt(argc, argv, "p:e:s:i:h")) != -1) {
        switch (opt) {
        case 'p':
            *pages = atoi(optarg);
            break;
        case 'e':
            *per_ev = atoi(optarg);
            break;
        case 's':
            *shrink_factor = atoi(optarg);
            break;
        case 'i':
            *interleave = atoi(optarg);
            break;
        case 'h':
            printf("Usage: %s %s [-p] [pages] [-e] [per_ev] [-s] [shrink_factor] [-i] [interleave]\n", argv[0], argv[1]);
        default:
            exit(EXIT_FAILURE);
        }
    }
}