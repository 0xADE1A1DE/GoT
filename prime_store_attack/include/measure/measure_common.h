#pragma once
#include <stdint.h>
#include "measure.h"
#include "util.h"

bool __always_inline common_is_in_cache(uint64_t result, measure_metadata params) {
    return result < params.threshold;
}

uint64_t __always_inline common_probe_once(node *set, uintptr_t candidate, measure_metadata params) {
    uintptr_t trash = 0;

    trash = FORCE_READ(candidate, trash);
    node *temp = set;
    while (temp != NULL) {
        trash = FORCE_READ(temp, trash);
        temp = temp->next;
    }
    temp = (node *)((uintptr_t)set | (trash == 0xbaaaaad));
    while (temp != NULL) {
        trash = FORCE_READ(temp, trash);
        temp = temp->next;
    }
    return params.measure(candidate, trash);
}

bool __always_inline common_probe(node *set, uintptr_t candidate, measure_metadata params) {
    uint32_t total = 0;
    for (int i = 0; i < params.probe_trials; i++) {
        uint64_t result = common_probe_once(set, candidate, params);
        total += !common_is_in_cache(result, params);
    }
    return total > (params.probe_trials / 2);
}