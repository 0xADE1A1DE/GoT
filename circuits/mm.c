#include <stdlib.h>
#include <assert.h>

#include "mm.h"

extern uintptr_t temporary_memory[TEMP_MEMORY_SIZE];
extern uintptr_t available_temporary_memory;

static uintptr_t enter_number = TEMP_MEMORY_SIZE;

uintptr_t get_temporary_memory() {
    if (available_temporary_memory == 0) {
        printf("Out of temporary memory!\n");
        while(1) {}
    }
    return temporary_memory[--available_temporary_memory];
    
}

void return_temporary_memory(uintptr_t offset) {
    temporary_memory[available_temporary_memory++] = offset; 
    
}

void enter_scope() {
  enter_number = available_temporary_memory;
}

void exit_scope() {
  assert(available_temporary_memory == enter_number);
}
