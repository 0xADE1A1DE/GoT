#pragma once



#ifdef GATES_STATS
  extern double circuit_execution_time_us;
  extern uintptr_t gates_count;
  extern uintptr_t intermediate_values;
#endif

#define FORCE_READ(addr, trash) (*(uintptr_t *)((uintptr_t)(addr) | (trash == 0xbaaaaad)))
#define FORCE_READ_PLAIN(addr) (*(uintptr_t *)((uintptr_t)(addr)))
#define TEMPORAL_ADD(trash, value) ((trash) += (value))



#define CALCULATE_OFFSET(i) ((i) * 320)

typedef enum {
    L1,
    L2,
    LLC,
    RAM,
    IN_CACHE
} address_state;


void __always_inline mfence() {
    asm volatile("mfence");
}

void __always_inline lfence() {
    asm volatile("lfence");
}

uint64_t __always_inline rdtsc() {
	uint32_t a, d;
	__asm__ volatile (
    "mfence \n"
    
	"rdtsc\n"
	
    "lfence \n"
	"mov %%edx, %0\n"
	"mov %%eax, %1\n"
	: "=r" (a), "=r" (d)
	:: "%rax", "%rdx");
	return ((uint64_t)a << 32) | d;
}

void __always_inline clflush(uintptr_t p)
{
    __asm__ volatile ("clflush 0(%0)" : : "c" (p) : "rax");
}



uintptr_t read_addr(uintptr_t addr, uintptr_t* output, uintptr_t trash);

void fetch_address(uintptr_t address, address_state to_state);
void print_state(address_state state);

void randomise_temp_memory();