#ifndef __MM_H__
#define __MM_H__

#include "consts.h"

#include "gates.h"



#define TEMP_MEMORY_SIZE 30000

uintptr_t get_temporary_memory();
void return_temporary_memory(const uintptr_t offset);
void enter_scope();
void exit_scope();

#endif