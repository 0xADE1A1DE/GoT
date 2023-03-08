#include <stdio.h>
#include "ev/reduce.h"
#include "measure/measure.h"
#include "consts.h"

#define BACKTRACKING_MAXIMUM (20)
uint32_t backtrack_count = 0;

bool reduce_eviction_set_rec(node *sentinel, uint32_t length, uintptr_t evictee);

bool reduce_eviction_set(node *sentinel, uint32_t length, uintptr_t evictee) {
    backtrack_count = 0;
    return reduce_eviction_set_rec(sentinel, length, evictee);
}

bool reduce_eviction_set_rec(node *sentinel, uint32_t length, uintptr_t evictee) {
    if (length == CACHE_ASSOCIATIVITY)
        return true;

    #define N (CACHE_ASSOCIATIVITY + 1)
    uint32_t advance_ceil = (length + N - 1) / N;
    uint32_t advance_floor = length / N;
    uint32_t advance = advance_ceil;

    node *chain_start;
    node *chain_end = sentinel;

    for (uint32_t i = 0; i < N; i++) {
        if (i == (length % N))
            advance = advance_floor;
        if (advance == 0)
            continue;
        chain_start = chain_end->next;
        chain_end = node_advance(chain_start, advance - 1);

        node *link_point = chain_start->prev;
        node_unlink_chain(chain_start, chain_end);

        // TODO: allow specifying a different probe function.
        if (rdtsc_measure_metadata.probe(sentinel->next, evictee)) {
            #ifdef REDUCE_DBG
            printf("Going in with %d at idx %d\n", length - advance, i);
            #endif
            if (reduce_eviction_set_rec(sentinel, length - advance, evictee))
                return true;
            if (backtrack_count >= BACKTRACKING_MAXIMUM) {
                return false;
            }
        }
        #ifdef REDUCE_DBG
        else {
            printf("Not going in..\n");
        }
        #endif
        node_link_chain(link_point, chain_start, chain_end);
    }
    backtrack_count++;
    #ifdef REDUCE_DBG
    printf("backtracking... %d\n", backtrack_count);
    if (backtrack_count == BACKTRACKING_MAXIMUM) {
        printf("Reached max with size %d\n", length);
    }
    #endif
    return false;
}