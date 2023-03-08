#include "ev/ev.h"
#include "ev/reduce.h"
#include "consts.h"
#include "util.h"

#define INITIAL_SAMPLE_SIZE (3000)
#define ALLOCATION_SIZE (4096 * PAGE_SIZE)

void *eviction_set_memory = NULL;

void initialize_candidates(node_array *candidates, int page_offset);

bool populate_eviction_set(eviction_set *llc_eviction_set, uintptr_t evictee) {
    node_array candidates = {0};
    if (eviction_set_memory == NULL) {
        initialize_allocation(&eviction_set_memory, ALLOCATION_SIZE);
    }

    int page_offset = evictee & (PAGE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);

    initialize_candidates(&candidates, page_offset);
    node_array_shuffle(candidates);

    candidates.length = INITIAL_SAMPLE_SIZE;
    node _sentinel = {0};
    node *sentinel = &_sentinel;

    node_array_to_linked_list(candidates, sentinel);
    node_array _llc_eviction_set = {0};

    free(candidates.arr);

    if (reduce_eviction_set(sentinel, INITIAL_SAMPLE_SIZE, evictee)) {
        _llc_eviction_set.length = CACHE_ASSOCIATIVITY;
        _llc_eviction_set.arr = malloc(_llc_eviction_set.length * sizeof(node *));
        linked_list_to_node_array(sentinel->next, _llc_eviction_set);

        llc_eviction_set->arr = (uintptr_t *)_llc_eviction_set.arr;
        llc_eviction_set->length = _llc_eviction_set.length;
        llc_eviction_set->eviction_set_page_offset = page_offset;
        return true;
    }
    return false;
}

bool compare_buckets(cache_bucket first, cache_bucket second) {
    return (first.cache_set == second.cache_set) && (first.slice == second.slice);
}

bool is_new_bucket(eviction_set *eviction_sets, int length, cache_bucket candidate_bucket) {
    for (int j = 0; j < length; j++) {
        cache_bucket compared = to_cache_bucket(eviction_sets[j].arr[0]);
        if (compare_buckets(candidate_bucket, compared))
            return false;
    }
    return true;
}

eviction_set *find_eviction_sets_cheat_find_one(int allocation_page_index) {
    uintptr_t first_ev_member = (uintptr_t)eviction_set_memory + allocation_page_index * PAGE_SIZE;
    cache_bucket bucket = to_cache_bucket(first_ev_member);
    eviction_set *current_eviction_set = malloc(sizeof(eviction_set));

    current_eviction_set->length = CACHE_ASSOCIATIVITY;
    current_eviction_set->eviction_set_page_offset = 0;
    current_eviction_set->arr = malloc(CACHE_ASSOCIATIVITY * sizeof(node *));
    current_eviction_set->arr[0] = first_ev_member;
    
    int current_ev_index = 1;
    for (int j = allocation_page_index + 1; (j < ALLOCATION_SIZE / PAGE_SIZE) && (current_ev_index < CACHE_ASSOCIATIVITY); j++) {
        uintptr_t current = (uintptr_t)eviction_set_memory + j * PAGE_SIZE;
        if (compare_buckets(bucket, to_cache_bucket(current)))
            current_eviction_set->arr[current_ev_index++] = current;
    }
    if (current_ev_index == CACHE_ASSOCIATIVITY)
        return current_eviction_set;
    
    // No success, return NULL.
    free(current_eviction_set->arr);
    free(current_eviction_set);
    return NULL;
}

eviction_set *find_eviction_sets(int amount) {
    if (eviction_set_memory == NULL) {
        initialize_allocation(&eviction_set_memory, ALLOCATION_SIZE);
    }

    eviction_set *eviction_sets = malloc(amount * sizeof(eviction_set));
    int current_amount = 0;
    for (int i = 0; (i < ALLOCATION_SIZE / PAGE_SIZE) && (current_amount < amount); i++) {
        uintptr_t current_representative_candidate = (uintptr_t)eviction_set_memory + i * PAGE_SIZE;
        cache_bucket current_bucket = to_cache_bucket(current_representative_candidate);

        // ensure it's a new representative
        if (!is_new_bucket(eviction_sets, current_amount, current_bucket))
            continue;
        
        eviction_set *result = find_eviction_sets_cheat_find_one(i);
        if (result == NULL)
            continue;
        eviction_sets[current_amount++] = *result;
        free(result);
    }
    if (current_amount == amount)
        return eviction_sets;
    
    // This leaks, but who cares.
    free(eviction_sets);
    return NULL;
}

void initialize_candidates(node_array *candidates, int page_offset) {
    // Note: the -1 at the end isn't strictly necessary, but it's there to avoid trouble in creating l1_eviction_set.
    candidates->length = ALLOCATION_SIZE / PAGE_SIZE - 1;
    candidates->arr = malloc(candidates->length * sizeof(node *));
    for (int i = 0; i < candidates->length; i++) {
        candidates->arr[i] = (node *)(eviction_set_memory + i * PAGE_SIZE + page_offset);
    }
}