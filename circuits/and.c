#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t and_fan_gate_impl(uintptr_t input1, uintptr_t input2, uintptr_t output, bool wet_run, uintptr_t trash) {
  
  
    
    
  
  
  
  
  fetch_address(&input1, L1);
  fetch_address(&input2, L1);
  fetch_address(&output, L1);
  fetch_address(&wet_run, L1);
  fetch_address(&trash, L1);

  
  
  
  
  
  
  
  
  
  
  trash = FORCE_READ(input1 - 128, trash);
  trash = FORCE_READ(input2 - 128, trash);
  trash = FORCE_READ(output - 128, trash);
  

  mfence();
  lfence();
  
    const double start = DBL_MIN;
    const double denormal_result = DBL_MIN / 2;

    double divide_by = wet_run + 1;
    double result = start / divide_by;

    if (result == denormal_result)
        return trash;
    
    
    asm volatile("");
    
    

    uintptr_t val1 = FORCE_READ(input1, trash);
    uintptr_t val2 = FORCE_READ(input2, trash);
    
    
    
    
    
    

    
    
    

    trash = val1 | val2;
    uintptr_t sum = 0;
  
    sum += *(uintptr_t *)(output + trash);
    return trash + sum;
}

uintptr_t and_fan_gate_impl_2(uintptr_t input1, uintptr_t input2, uintptr_t output1, uintptr_t output2, bool wet_run, uintptr_t trash) {
  
  
    
  
  
  
  
  
  
  
  input1 = input1 | (input1 == 0xbaaaaad);
  input2 = input2 | (input2 == 0xbaaaaad);
  output1 = output1 | (output1 == 0xbaaaaad);
  output2 = output2 | (output2 == 0xbaaaaad);
  wet_run = wet_run | (wet_run == 0xbaaaaad);
  trash = trash | (trash == 0xbaaaaad);
  
  
  
  
  
  
  
  trash = FORCE_READ(input1 - 128, trash);
  trash = FORCE_READ(input2 - 128, trash);
  trash = FORCE_READ(output1 - 128, trash);
  trash = FORCE_READ(output2 - 128, trash);
  

  mfence();
  lfence();
    
    const double start = DBL_MIN;
    const double denormal_result = DBL_MIN / 2;

    double divide_by = wet_run + 1;
    double result = start / divide_by;

    if (result == denormal_result)
        return trash;
    
    
    asm volatile("");
    if (!wet_run)
        return trash;

    
    
    

    uintptr_t val1 = FORCE_READ(input1, trash);
    uintptr_t val2 = FORCE_READ(input2, trash);
    
    
    
    
    
    
    trash = val1 + val2;
    uintptr_t sum = 0;
    
    sum += *(uintptr_t *)(output1 + trash);
    sum += *(uintptr_t *)(output2 + trash);
    return trash + sum;
}

uintptr_t and_fan_gate(uintptr_t input1, uintptr_t input2, uintptr_t output, uintptr_t trash) {
  
#ifdef GATES_STATS
  gates_count++;
  intermediate_values += 1;
#endif

  gate_metadata __attribute__ ((aligned (64))) fake = {0};  


  trash = and_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl(input1, input2, output, true, trash);
  
  return trash;
}

uintptr_t and_fan_gate_2(uintptr_t input1, uintptr_t input2, uintptr_t output1, uintptr_t output2, uintptr_t trash) {
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 2;
  #endif
  
  
  gate_metadata __attribute__ ((aligned (64))) fake = {0};  


  trash = and_fan_gate_impl_2(&fake, &fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl_2(&fake, &fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl_2(&fake, &fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl_2(&fake, &fake, &fake, &fake, false, trash);
  trash = and_fan_gate_impl_2(input1, input2, output1, output2, true, trash);
  
  return trash;
} 

uintptr_t test_and_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state) {
  
  
  uintptr_t correct = 0;
  uintptr_t incorrect = 0;
  
  uintptr_t output_read;
  
  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t output = get_temporary_memory();
  
  for (uintptr_t i = 0; i < iteration; i++) {
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  trash = and_fan_gate(input1, input2, output, trash);
  
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  
  fetch_address(input1, state);

  mfence();
  lfence();
  
  trash = and_fan_gate(input1, input2, output, trash);
  
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  
  fetch_address(input2, state);
  mfence();
  lfence();
  
  trash = and_fan_gate(input1, input2, output, trash);
  
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  
  

  fetch_address(input1, state);
  fetch_address(input2, state);
  
  mfence();
  lfence();

  trash = and_fan_gate(input1, input2, output, trash);
  
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 1)
    correct++;
  else
    incorrect++;
  
  }
  printf("and ACCURACY : %ld / %ld ", correct, correct + incorrect);
  print_state(state);
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  
  return trash;
}

uintptr_t test_and_2_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state) {
  
  
  uintptr_t correct = 0;
  uintptr_t incorrect = 0;
  
  uintptr_t output_1_read;
  uintptr_t output_2_read;
  
  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t output1 = get_temporary_memory();
  uintptr_t output2 = get_temporary_memory();
  
  for (uintptr_t i = 0; i < iteration; i++) {
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  trash = and_fan_gate_2(input1, input2, output1, output2, trash);
  
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  if (output_1_read == 0)
    correct++;
  else
    incorrect++;
    
  if (output_2_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  
  fetch_address(input1, state);

  mfence();
  lfence();
  
  trash = and_fan_gate_2(input1, input2, output1, output2, trash);
  
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  if (output_1_read == 0)
    correct++;
  else
    incorrect++;
    
  if (output_2_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  
  fetch_address(input2, state);
  mfence();
  lfence();
  
  trash = and_fan_gate_2(input1, input2, output1, output2, trash);
  
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  if (output_1_read == 0)
    correct++;
  else
    incorrect++;
    
  if (output_2_read == 0)
    correct++;
  else
    incorrect++;
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  
  

  fetch_address(input1, state);
  fetch_address(input2, state);
  
  mfence();
  lfence();

  trash = and_fan_gate_2(input1, input2, output1, output2, trash);
  
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  if (output_1_read == 1)
    correct++;
  else
    incorrect++;
    
  if (output_2_read == 1)
    correct++;
  else
    incorrect++;
  
  }
  printf("and 2 ACCURACY : %ld / %ld ", correct, correct + incorrect);
  print_state(state);
  return_temporary_memory(output2);
  return_temporary_memory(output1);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  
  return trash;
}