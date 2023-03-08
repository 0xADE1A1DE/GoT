#pragma once
#include "consts.h"
#include "linked_list.h"
#include <stddef.h>
#include <stdint.h>

namespace ev {

#define DEFAULT_ALLOCATION_SIZE (3 * LLC_CACHE_SIZE)

class EvictionSet {
public:
    uintptr_t arr[LLC_CACHE_ASSOCIATIVITY];

    int eviction_set_page_offset() {
        return arr[0] % PAGE_SIZE_;
    }
};

template<int PROBE_TRIALS, class MEASURE_CLASS>
class ProbeParamters {
public:
    static constexpr int probe_trials = PROBE_TRIALS;
    using MeasureClass = MEASURE_CLASS;
};

class EvictionSetManager {
public:
    EvictionSetManager(size_t allocation_size = DEFAULT_ALLOCATION_SIZE);

    EvictionSet *reduce_eviction_set(uintptr_t target, int retries = 1);
    EvictionSet **find_eviction_sets(int per_bucket, int amount);

    static int get_page_offset(uintptr_t address) {
        return address % PAGE_SIZE_;
    }

protected:
    uintptr_t eviction_set_memory;

private:
    template<class probe_paramaters>
    bool reduce_eviction_set(node *sentinel, uint32_t length, uintptr_t evictee, typename probe_paramaters::MeasureClass m);
};

}