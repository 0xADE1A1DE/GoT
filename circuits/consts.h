#pragma once

#define PAGE_BITS (12)
#define PAGE_SIZE (4096)
#define CACHE_LINE_BITS (6)
#define CACHE_LINE_SIZE (64)

// Processor specific definitions
#define LLC_CACHE_SIZE (6*1024*1024)
#define CACHE_ASSOCIATIVITY (12)
#define CACHE_SLICES (8)
#define LINE_FILL_BUFFER_SIZE (12)

// Stands for 1.6GHZ, RDTSC based timings divided by this constant converts the timing into ms.
#define PROCESSOR_CLOCK_SPEED_MS (1600000)
#define RDTSC_THRESHOLD (180)

// The LINE_FILL_BUFFER_SIZE is the maximum, in practice, it is safer to use a smaller value.
#define GATE_FANNING (3)

// The LLC_ALLOCATION_MULTIPLIER is chosen such that a readthrough of an allocation of size `ALLOCATION_SIZE` will evict all of the LLC with extremely high probability.
// #define LLC_ALLOCATION_MULTIPLIER (1000)
// #define ALLOCATION_SIZE (LLC_ALLOCATION_MULTIPLIER * LLC_CACHE_SIZE)

#define ALLOCATION_SIZE 1024 * 1024 * 1024

extern int64_t SLOW_PARAM;

#define GATE_REPEATS 100

//#define TARGET_ACCURACY 0.90

#define GAME_OF_LIFE_DIMENSION 12