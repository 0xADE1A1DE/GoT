#pragma once
#include <stdint.h>
#include <stdbool.h>

/* TODO: document this */

typedef struct _amplification_metadata amplification_metadata;

typedef uintptr_t (*initialize_amplification_func_type)(amplification_metadata *metadata, uintptr_t trash);
typedef uintptr_t (*do_amplification_func_type)(uintptr_t target, uintptr_t trash);
// The verification scheme must verify the direct data accesses it entails (excluding stack / other out of direct control accesses), 
// modoulu PAGE_SIZE, are only on affected_double_cache_lines.
typedef bool (*verify_amplification_func_type)(amplification_metadata *metadata);

struct _amplification_metadata{
    initialize_amplification_func_type initialize;
    do_amplification_func_type amplify;
    verify_amplification_func_type verify;
    bool affected_double_cache_lines[32];
};