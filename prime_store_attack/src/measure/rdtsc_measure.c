#include "measure/measure_common.h"
#include "util.h"

#define RDTSC_THRESHOLD (180)

uint64_t rdtsc_measure(uintptr_t candidate, uintptr_t trash) {
    uint64_t start = rdtsc();
    TEMPORAL_ADD(trash, start);
    trash = FORCE_READ(candidate, trash);
    uint64_t end = rdtsc();
    TEMPORAL_ADD(trash, end);
    return end - start + (trash == 0xbaaaaad);
}

bool rdtsc_is_in_cache(uint64_t result);
bool rdtsc_probe(node *set, uintptr_t candidate);

const measure_metadata rdtsc_measure_metadata = {
    .probe_trials = 7,
    .threshold = RDTSC_THRESHOLD,
    .units = UNITS_RDTSC,
    .measure = rdtsc_measure,
    .is_in_cache = rdtsc_is_in_cache,
    .probe = rdtsc_probe
};

bool __attribute__ ((noinline)) rdtsc_is_in_cache(uint64_t result) {
    return common_is_in_cache(result, rdtsc_measure_metadata);
}

bool __attribute__ ((noinline)) rdtsc_probe(node *set, uintptr_t candidate) {
    return common_probe(set, candidate, rdtsc_measure_metadata);
}