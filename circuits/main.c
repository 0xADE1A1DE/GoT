#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <sys/mman.h>

#include "consts.h"
#include "gates.h"
#include "mm.h"
#include "util.h"

#ifdef MAP_HUGETLB
#define HUGEPAGES MAP_HUGETLB
#endif
#ifdef VM_FLAGS_SUPERPAGE_SIZE_2MB
#define HUGEPAGES VM_FLAGS_SUPERPAGE_SIZE_2MB
#endif

#ifdef HUGEPAGES
#define HUGEPAGEBITS 21
#define HUGEPAGESIZE (1<<HUGEPAGEBITS)
#define HUGEPAGEMASK (HUGEPAGESIZE - 1)
#endif

#define MAP_HUGE_1GB (30 << MAP_HUGE_SHIFT)


#ifdef GATES_STATS
  double circuit_execution_time_us = 0.00;
  uintptr_t gates_count = 0;
  uintptr_t intermediate_values = 0;
#endif

void *memory = NULL;
void *clear_memory = NULL;

uintptr_t temporary_memory[TEMP_MEMORY_SIZE];
uintptr_t available_temporary_memory;

uintptr_t read_addr(uintptr_t addr, uintptr_t *output, uintptr_t trash)
{
  uint64_t start = rdtsc();
  TEMPORAL_ADD(trash, start);
  trash = FORCE_READ(addr, trash);
  uint64_t end = rdtsc();
  TEMPORAL_ADD(trash, end);
  *output = (end - start) <= RDTSC_THRESHOLD;

  return trash;
}

#define ACCURACY_TEST_TIME 10000

static void swap(uintptr_t *a, uintptr_t *b)
{
  uintptr_t temp = *a;
  *a = *b;
  *b = temp;
}

void randomise_temp_memory()
{
  for (int i = 0; i < TEMP_MEMORY_SIZE; i++)
  {
    if (rand() & 1)
    {
      uintptr_t idx = rand() % TEMP_MEMORY_SIZE;
      swap(&temporary_memory[i], &temporary_memory[idx]);
    }
  }
}

uint64_t TARGET_SUCCESS_COUNTS = 7;
int64_t SLOW_PARAM = 13;

intptr_t prefetcher_enabled = 0;

int main(int argc, char** argv)
{
  if (argc >= 4) 
    TARGET_SUCCESS_COUNTS = atoi(argv[3]);
  if (argc >= 5) 
    SLOW_PARAM = atoi(argv[4]);
  if (argc >= 6 && !strcmp(argv[5], "prefetch"))
    prefetcher_enabled = 1;

  
  
  srand(time(NULL));
  uintptr_t trash = 0;

  memory = mmap(NULL, ALLOCATION_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS| MAP_HUGETLB | MAP_HUGE_1GB, 0, 0);
  
  for (uintptr_t i = 0; i < ALLOCATION_SIZE; i++)
  {
    *(uint8_t *)(memory + i) = 0;
  }

  uintptr_t bitmap[(TEMP_MEMORY_SIZE / (sizeof(uintptr_t) * 8)) + 1] = {0};

  for (int i = 0; i < TEMP_MEMORY_SIZE; i++) {
    
    uintptr_t bitlen = sizeof(uintptr_t) * 8;
    uintptr_t idx;
    do {
      idx = rand() % TEMP_MEMORY_SIZE;
    } while (bitmap[idx / bitlen] & ((uintptr_t)1 << (idx % bitlen)));
    temporary_memory[i] = memory + CALCULATE_OFFSET(idx) + 128;
    
    
    bitmap[idx / bitlen] |= ((uintptr_t)1 << (idx % bitlen));
    for (int i = 0; i < sizeof(bitmap) / sizeof(uintptr_t); i++) {
      
    }
    
    printf("%ld,", idx);
  }
  printf("\n");

  available_temporary_memory = TEMP_MEMORY_SIZE;
  
  #define REP 100

  if (!strcmp(argv[1], "sha1"))
    trash = sha1(trash, REP);
  else if(!strcmp(argv[1], "alu"))
    trash = test_alu_accuracy(trash, REP);
  else if (!strcmp(argv[1], "alu_maj"))
    trash = test_alu_accuracy_majority(trash, REP);
  else if (!strcmp(argv[1], "gol_glider")) {
    if (argc < 3) {
      printf("Please specify number of iterations!\n");
      munmap(memory, ALLOCATION_SIZE);
      return trash;
    }
    trash = gol_game_glider(trash, REP, atoi(argv[2]));
  }
  else if (!strcmp(argv[1], "gol_glider_demo")) {
    TARGET_SUCCESS_COUNTS = 1;
    for (int i = 0; i < 20; i++) {
      trash = gol_game_glider(trash, 30, i);
    }
  }
  
  munmap(memory, ALLOCATION_SIZE);
  return trash;
}