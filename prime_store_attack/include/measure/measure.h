#pragma once
#include <stdbool.h>
#include "linked_list.h"

typedef uint64_t (*measure_access_time_func_type)(uintptr_t candidate, uintptr_t trash);
typedef bool (*determine_in_cache_func_type)(uint64_t result);
typedef bool (*probe_func_type)(node *set, uintptr_t candidate);

typedef enum {
    UNITS_RDTSC,
    UNITS_100US,
} measure_units;

typedef struct {
    int probe_trials;
    uint64_t threshold;
    measure_units units;
    measure_access_time_func_type measure;
    determine_in_cache_func_type is_in_cache;
    probe_func_type probe;
} measure_metadata;

extern const measure_metadata tree_amplification_100us_measure_metadata;
extern const measure_metadata rdtsc_measure_metadata;