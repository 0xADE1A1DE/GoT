#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t branchless_fan_gate_impl(uintptr_t input, uintptr_t out1, uintptr_t out2, bool wet_run, uintptr_t trash) {
    
    
    
    
    
    
    gate_metadata __attribute__ ((aligned (64))) fake = {0};
    fake.base_ptr = (uintptr_t)&fake;

    
    const double start = DBL_MIN;
    const double denormal_result = DBL_MIN / 2;

    double divide_by = wet_run + 1;
    double result = start / divide_by;

    
    uintptr_t mask_fake = -(uintptr_t)(result == denormal_result);
    uintptr_t mask_out = -(uintptr_t)(result != denormal_result);
    
    uintptr_t out_1_deref = (((uintptr_t)&out1) & mask_out) | (((uintptr_t)&fake) & mask_fake);
    uintptr_t out_2_deref = (((uintptr_t)&out2) & mask_out) | (((uintptr_t)&fake) & mask_fake);
    uintptr_t *out1_final = *((uintptr_t *) out_1_deref);
    uintptr_t *out2_final = *((uintptr_t *) out_2_deref);

    trash = *(uintptr_t *)(input);
    uintptr_t sum = 0;
    
    
    
    
    sum += FORCE_READ(out1_final + trash, trash);
    sum += FORCE_READ(out2_final + trash, trash);
    return trash + sum;
}

uintptr_t branchless_fan_gate(uintptr_t input, uintptr_t out1, uintptr_t out2, uintptr_t trash) {
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 2;
  #endif
  return branchless_fan_gate_impl(input, out1, out2, true, trash);
} 

uintptr_t test_branchless_fan(uintptr_t trash) {
  {
  
  printf("BRANCHLESS FAN GATE: \n");
  
  uintptr_t output1_read;
  uintptr_t output2_read;
  
  uintptr_t input = get_temporary_memory();
  uintptr_t output1 = get_temporary_memory();
  uintptr_t output2 = get_temporary_memory();
  
  
  
  mfence();
  lfence();
  
  clflush(input);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  trash = branchless_fan_gate(input, output1, output2, trash);
  
  trash = read_addr(output1, &output1_read, trash);
  trash = read_addr(output2, &output2_read, trash);
  
  printf("0 : %d %d\n", output1_read, output2_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input, trash);
  
  trash = branchless_fan_gate(input, output1, output2, trash);
  
  trash = read_addr(output1, &output1_read, trash);
  trash = read_addr(output2, &output2_read, trash);
  
  printf("1 : %d %d\n", output1_read, output2_read);
  
  return_temporary_memory(output2);
  return_temporary_memory(output1);
  return_temporary_memory(input);
  
  
  
  }
  return trash;
}