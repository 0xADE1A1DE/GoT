#include "measure/measure_common.h"
#include "amplification/tree_amplification.h"
#include "util.h"

#define TREE_AMPLIFICATION_100us_100us_THRESHOLD (4)
#define TREE_AMPLIFICATION_1MS_RDTSC_THRESHOLD (4980744)

typedef uint64_t (*timestamp_func)();

uint64_t __always_inline tree_amplification_measure(uintptr_t candidate, uintptr_t trash, timestamp_func timestamp, amplification_metadata tree_amplification) {
    uint64_t start = timestamp();
    TEMPORAL_ADD(trash, start);
    trash = tree_amplification.amplify(candidate, trash);
    uint64_t end = timestamp();
    TEMPORAL_ADD(trash, end);
    return end - start + (trash == 0xbaaaaad);
}

uint64_t tree_amplification_1ms_measure(uintptr_t candidate, uintptr_t trash);
bool tree_amplification_1ms_is_in_cache(uint64_t result);
bool tree_amplification_1ms_probe(node *set, uintptr_t candidate);

const measure_metadata tree_amplification_1ms_measure_metadata = {
    .probe_trials = 7,
    .threshold = TREE_AMPLIFICATION_1MS_RDTSC_THRESHOLD,
    .units = UNITS_RDTSC,
    .measure = tree_amplification_1ms_measure,
    .is_in_cache = tree_amplification_1ms_is_in_cache,
    .probe = tree_amplification_1ms_probe
};

uint64_t tree_amplification_1ms_measure(uintptr_t candidate, uintptr_t trash) {
    return tree_amplification_measure(candidate, trash, rdtsc, tree_amplification_1ms);
}

bool __attribute__ ((noinline)) tree_amplification_1ms_is_in_cache(uint64_t result) {
    return common_is_in_cache(result, tree_amplification_1ms_measure_metadata);
}

bool __attribute__ ((noinline)) tree_amplification_1ms_probe(node *set, uintptr_t candidate) {
    return common_probe(set, candidate, tree_amplification_1ms_measure_metadata);
}


uint64_t tree_amplification_100us_measure(uintptr_t candidate, uintptr_t trash);
bool tree_amplification_100us_is_in_cache(uint64_t result);
bool tree_amplification_100us_probe(node *set, uintptr_t candidate);

const measure_metadata tree_amplification_100us_measure_metadata = {
    .probe_trials = 7,
    .threshold = TREE_AMPLIFICATION_100us_100us_THRESHOLD,
    .units = UNITS_100US,
    .measure = tree_amplification_100us_measure,
    .is_in_cache = tree_amplification_100us_is_in_cache,
    .probe = tree_amplification_100us_probe
};

uint64_t tree_amplification_100us_measure(uintptr_t candidate, uintptr_t trash) {
    return tree_amplification_measure(candidate, trash, time_in_100us, tree_amplification_100us);
}

bool __attribute__ ((noinline)) tree_amplification_100us_is_in_cache(uint64_t result) {
    return common_is_in_cache(result, tree_amplification_100us_measure_metadata);
}

bool __attribute__ ((noinline)) tree_amplification_100us_probe(node *set, uintptr_t candidate) {
    return common_probe(set, candidate, tree_amplification_100us_measure_metadata);
}