#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#define CLEAR_CACHE_MEMORY_MULTIPLIER (14)

#define FORCE_READ(addr, trash) (*(uintptr_t *)((uintptr_t)(addr) | (trash == 0xbaaaaad)))
#define TEMPORAL_ADD(trash, value) ((trash) += (value))

#define ADDRESS_STATES (4)

typedef enum {
    L1,
    L2,
    LLC,
    RAM,
    IN_CACHE
} address_state;

typedef struct {
    int cache_set;
    int slice;
} cache_bucket;

void __always_inline clflush(void *p)
{
    __asm__ volatile ("clflush 0(%0)" : : "c" (p) : "rax");
}

uint64_t __always_inline time_in_100us() {
    struct timeval te;
    gettimeofday(&te, NULL);
    return te.tv_sec*10000LL + te.tv_usec/100LL;
}

uint64_t __always_inline rdtsc() {
	uint32_t a, d;
	__asm__ volatile (
    "mfence \n"
    "lfence \n"
	"rdtsc\n"
	// "mfence \n"
    "lfence \n"
	"mov %%edx, %0\n"
	"mov %%eax, %1\n"
	: "=r" (a), "=r" (d)
	:: "%rax", "%rdx");
	return ((uint64_t)a << 32) | d;
}

#define PROCESSOR_CLOCK_SPEED_MS (1600000)

double __always_inline rdtsc_to_ms(uint64_t rdtsc) {
    double result = ((double)rdtsc) / PROCESSOR_CLOCK_SPEED_MS;
    return result;
}

int __always_inline get_rand_range(int lower, int upper) {
    return rand() % (upper + 1 - lower) + lower;
}

cache_bucket to_cache_bucket(uint64_t vaddr);
void print_cache_bucket(uint64_t vaddr);

uint64_t int_pow(uint64_t base, unsigned int exp);

void initialize_allocation(void **target, size_t size);
void initialize_clear_cache_allocation();
uintptr_t clear_all_caches(uintptr_t trash);

void fetch_address(uintptr_t address, address_state to_state);
char *state_to_string(address_state state);

void hexdump(const char * desc, const void * addr, const int len, int perLine);
uintptr_t mmap_symbol_from_binary(char *path, char *symbol);