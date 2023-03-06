#include <stdint.h>
#include <stddef.h>

#include "util.h"

void fetch_address(uintptr_t address, address_state to_state) {
    switch (to_state)
    {
    case L1: asm volatile ("prefetcht0 (%0)" : : "r" (address)); break;
    case L2: asm volatile ("prefetcht1 (%0)" : : "r" (address)); break;
    case LLC: asm volatile ("prefetcht2 (%0)" : : "r" (address)); break;
    case RAM: clflush((void *)address); break;
    case IN_CACHE: asm volatile ("prefetcht2 (%0)" : : "r" (address)); break;
    }
}

void print_state(address_state state) {
  switch (state) {
    case L1: printf("fetch to L1\n"); break;
    case L2: printf("fetch to L2\n"); break;
    case LLC: case IN_CACHE: printf("fetch to LLC\n"); break;
    case RAM: printf("in RAM\n"); break;
  }
  return;
}