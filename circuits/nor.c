#include <stdlib.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t nor_fan_gate_impl(volatile uintptr_t input1, volatile uintptr_t input2, volatile uintptr_t output, volatile bool wet_run, volatile uintptr_t trash)
{
  








  
    for (int i = 0; i < 128; i++)
    {
      asm volatile("");
    }
    
  
  
  
  
  

  input1 = input1 | (input1 == 0xbaaaaad);
  input2 = input2 | (input2 == 0xbaaaaad);
  output = output | (output == 0xbaaaaad);
  wet_run = wet_run | (wet_run == 0xbaaaaad);
  trash = trash | (trash == 0xbaaaaad);
  
  
  
  
  
  
  trash = FORCE_READ(input1 - 128, trash);
  trash = FORCE_READ(input2 - 128, trash);
  trash = FORCE_READ(output - 128, trash);
  

  mfence();
  lfence();
  
  
  
  
  volatile uintptr_t val1 = FORCE_READ_PLAIN(input1);
  volatile uintptr_t val2 = FORCE_READ_PLAIN(input2);
  uintptr_t new_trash = val1 + val2;
  if (wet_run == ((val1 != 0xbaaaaad)))
  {
    return new_trash;
  }
  if (wet_run == ((val2 != 0xbaaaaad)))
  {
    return new_trash;
  }

  
  asm volatile("");
  if (!wet_run)
    return new_trash;

  volatile float always_zero = (float)(wet_run - 1);
  
  
  #pragma GCC unroll 300
  for (int i = 0; i < SLOW_PARAM - 2; i++) { always_zero *= always_zero; }

  trash += *(uintptr_t *)((output + trash) | (always_zero != 0));
  return trash + new_trash;
}

uintptr_t nor_fan_gate(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash) {
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 1;
  #endif
  gate_metadata __attribute__ ((aligned (64))) fake = {0};  

  trash = nor_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = nor_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = nor_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = nor_fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = nor_fan_gate_impl(in1, in2, out, true, trash);
  
  return trash;
}

uintptr_t test_nor(uintptr_t trash) {
  {
  
  printf("NOR GATE: \n");
  
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  printf("1 1 : %d\n", output_read);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}

uintptr_t test_nor_accuracy(uintptr_t trash, uintptr_t iteration,
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 1)
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
  
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
  
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
  
  trash = nor_fan_gate(input1, input2, output, trash);
  
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 0)
    correct++;
  else
    incorrect++;
  
  }
  printf("NOR ACCURACY : %ld / %ld ", correct, correct + incorrect);
  print_state(state);
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  
  return trash;
}