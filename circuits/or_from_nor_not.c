#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t or_from_nor_not(volatile uintptr_t input1, volatile uintptr_t input2, volatile uintptr_t output, volatile uintptr_t trash) {
  volatile uintptr_t nor_out = get_temporary_memory();
  
  fetch_address(&input1, L1);
  fetch_address(&input2, L1);
  fetch_address(&output, L1);
  fetch_address(&trash, L1);
  fetch_address(&nor_out, L1);
  
  
  
  
  
  

  mfence();
  lfence();
  

  clflush(nor_out);
  mfence();

  trash = nor_fan_gate(input1, input2, nor_out, trash);
  
  mfence();
  lfence();

  trash = not_fan_gate(nor_out, output, trash);
  
  mfence();
  lfence();

  return_temporary_memory(nor_out);

  return trash;
}
uintptr_t test_or_from_nor_not(uintptr_t trash) {
  {
  
  printf("OR FROM NOR NOT GATE: \n");
  
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
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
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
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
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
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
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
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  printf("1 1 : %d\n", output_read);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}

uintptr_t test_or_from_nor_not_accuracy(uintptr_t trash, uintptr_t iteration) {
  {
  
  
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
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
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
  
  trash = FORCE_READ(input1, trash);
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
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
  
  trash = FORCE_READ(input2, trash);
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
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
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  trash = or_from_nor_not(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 1)
    correct++;
  else
    incorrect++;
  }
  printf("OR FROM NOR NOT Accuracy : %ld / %ld\n", correct, correct + incorrect);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}