#pragma once
#include <stdbool.h>
#include "linked_list.h"

extern void *eviction_set_memory;

typedef struct {
    uintptr_t *arr;
    uint32_t length;
    int eviction_set_page_offset;
} eviction_set;

bool populate_eviction_set(eviction_set *llc_eviction_set, uintptr_t evictee);
eviction_set *find_eviction_sets(int amount);