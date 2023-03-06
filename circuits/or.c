#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t __attribute__((optimize("fast"))) or_fan_gate_impl (uintptr_t in1, uintptr_t in2, uintptr_t out, bool wet_run, uintptr_t trash)   {
    
    
    
    const double start = DBL_MIN;
    const double denormal_result = DBL_MIN / 2;

    double divide_by = wet_run + 1;
    double result = start / divide_by;

    if (result == denormal_result)
        return trash;
    
    
    asm volatile("");
    if (!wet_run)
        return trash;

    
    uintptr_t val1 = FORCE_READ(in1, trash);
    uintptr_t val2 = FORCE_READ(in2, trash);
    volatile uintptr_t new_trash = ((val1 != 0xbaaaaad) + (val2 != 0xbaaaaad)) > 0;
    uintptr_t sum = 0;
    
    
    
    
    for (uintptr_t i = 0; i < SLOW_PARAM ; i++) { trash = trash * trash; }
    sum += *(uintptr_t *)(out);
    return trash + sum + new_trash;
}

uintptr_t or_fan_gate(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash) {
  gate_metadata __attribute__ ((aligned (64))) fake = {0};  


  trash = or_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = or_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = or_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = or_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = or_fan_gate_impl(in1, in2, out, true, trash);
  
  return trash;
}

uintptr_t test_or(uintptr_t trash) {
  {
  
  printf("OR GATE: \n");
  
  uintptr_t output_read;
  
  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t output = get_temporary_memory();
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  trash = or_fan_gate(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  printf("0 0 : %d\n", output_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  
  trash = or_fan_gate(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  printf("1 0 : %d\n", output_read);
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);
  
  trash = or_fan_gate(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  printf("0 1 : %d\n", output_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  trash = or_fan_gate(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  printf("1 1 : %d\n", output_read);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}