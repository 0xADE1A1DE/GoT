#pragma once

// Note: em++ also defines PAGE_SIZE, to the wasm page size which is 65536.
// As a compromise, I settled on PAGE_SIZE_
#define PAGE_SIZE_ (4096)

// Cache properties.
#define LLC_CACHE_SIZE (6*1024*1024)
#define LLC_CACHE_ASSOCIATIVITY (12)
#define L1_CACHE_ASSOCIATIVITY (8)
#define CACHE_SLICES (8)
#define CACHE_LINE_SIZE (64)
#define DOUBLE_CACHE_LINE_SIZE (128)

#define RDTSC_THRESHOLD (120)

// ASCII colors
//Regular text
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
//Reset
#define RST "\e[0m"