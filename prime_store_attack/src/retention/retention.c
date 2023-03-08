#include <assert.h>
#include "gates/bt_gates.h"
#include "gates/nbt_gates.h"
#include "retention/retention.h"
#include "util.h"
#include "consts.h"

uintptr_t get_retention_offset(retention_scheme *scheme, int index, int copy_index);

int total_allowed_double_cache_lines(retention_metadata metadata) {
    int result = 0;
    for (int i = 0; i < 32; i++) {
        if (metadata.allowed_double_cache_lines[i])
            result++;
    }
    return result;
}

int total_retention_entries(retention_metadata metadata) {
    return total_allowed_double_cache_lines(metadata) * metadata.pages;
}

int total_measure_entries(retention_metadata metadata) {
    return total_retention_entries(metadata) / metadata.shrink_factor;
}

retention_scheme initialize_retention_scheme(retention_metadata metadata, eviction_set *eviction_sets) {
    assert(metadata.pages > 24);  // Avoid prefetcher
    int used_eviction_sets = metadata.pages / metadata.per_ev;
    assert(used_eviction_sets <= 128); // Number of eviction sets we're finding.

    retention_scheme result = {.metadata=metadata, .retention_pages = NULL};
    result.retention_pages = malloc(sizeof(uintptr_t *) * metadata.pages);

    const int interleave = metadata.interleave;

    for (int base = 0; base < used_eviction_sets; base += interleave) {
        for (int ev = 0; ev < interleave; ev++) {
            for (int j = 0; j < metadata.per_ev; j++) {
                if (base * metadata.per_ev + interleave * j + ev < metadata.pages)
                    result.retention_pages[base * metadata.per_ev + interleave * j + ev] = eviction_sets[base + ev].arr[j];
            }
        }
    }

    // for (int i = 0; i < used_eviction_sets; i++) {
    //     for (int j = 0; j < metadata.per_ev; j++)
    //         result.retention_pages[metadata.per_ev * i + j] = eviction_sets[i].arr[j];
    //         // result.retention_pages[used_eviction_sets * j + i] = eviction_sets[i].arr[j];
    // }

    int current_idx = 0;
    for (int i = 0; i < 32; i++) {
        if (metadata.allowed_double_cache_lines[i])
            result._page_offsets[current_idx++] = i * DOUBLE_CACHE_LINE_SIZE;
    }
    return result;
}

uintptr_t retention_get_entry(retention_scheme *scheme, int index) {
    return scheme->retention_pages[index % scheme->metadata.pages] + scheme->_page_offsets[index / scheme->metadata.pages];
}

gate_metadata * nor_gates[] = {
    NULL,
    &bt_not_1to1_metadata,
    &bt_nor_2to1_metadata,
    &bt_nor_3to1_metadata,
    &bt_nor_4to1_metadata,
    &bt_nor_5to1_metadata,
    &bt_nor_6to1_metadata,
};

// NOTE: you should flip the is_in_cache result!
uint64_t retention_measure(retention_scheme *scheme, int index, const measure_metadata *measure, uintptr_t trash) {
    static gate_bank __attribute__ ((aligned (PAGE_SIZE))) retention_bank = {0};
    retention_bank.input_base = (uintptr_t)eviction_set_memory;
    retention_bank.output_base = (uintptr_t)eviction_set_memory;
    uintptr_t output = retention_get_entry(scheme, scheme->metadata.shrink_factor * index) + PAGE_SIZE / 2;
    clflush((void *)output);
    asm volatile("mfence\nlfence\n");

    for (int i = 0; i < scheme->metadata.shrink_factor; i++) {
        retention_bank.input_offsets[i] = retention_get_entry(scheme, (scheme->metadata.shrink_factor) * index + i) - (uintptr_t)eviction_set_memory;
    }
    retention_bank.output_offsets[0] = output - (uintptr_t)eviction_set_memory;

    trash = apply_gate(nor_gates[scheme->metadata.shrink_factor], &retention_bank, trash);

    return measure->measure(output, trash);
}