#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "ev/ev.h"
#include "measure/measure.h"

typedef struct {
    bool allowed_double_cache_lines[32];
    int pages;
    int per_ev;
    int shrink_factor;
    int interleave;
} retention_metadata;

int total_allowed_double_cache_lines(retention_metadata metadata);
int total_retention_entries(retention_metadata metadata);
int total_measure_entries(retention_metadata metadata);

typedef struct {
    retention_metadata metadata;
    uintptr_t _page_offsets[32];
    uintptr_t *retention_pages;
} retention_scheme;

retention_scheme initialize_retention_scheme(retention_metadata metadata, eviction_set * eviction_sets);
uintptr_t retention_get_entry(retention_scheme *scheme, int index);
uint64_t retention_measure(retention_scheme *scheme, int index, const measure_metadata *measure, uintptr_t trash);