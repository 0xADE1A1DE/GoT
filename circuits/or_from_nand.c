#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t or_from_nand_impl(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash) {
  
  uintptr_t in1Copies[2];
  uintptr_t in2Copies[2];
  
  uintptr_t nand1_out = get_temporary_memory();
  uintptr_t nand2_out = get_temporary_memory();
  
  in1Copies[0] = get_temporary_memory();
  in1Copies[1] = get_temporary_memory();
  
  in2Copies[0] = get_temporary_memory();
  in2Copies[1] = get_temporary_memory();
  
  
  
  
  clflush(in1Copies[0]);
  clflush(in1Copies[1]);
  mfence();
  
  trash = fan_gate(in1, in1Copies[0], in1Copies[1], trash);

  clflush(in2Copies[0]);
  clflush(in2Copies[1]);
  mfence();  

  trash = fan_gate(in2, in2Copies[0], in2Copies[1], trash);
  
  clflush(nand1_out);
  mfence();  
  
  trash = nand_fan_gate(in1Copies[0], in1Copies[1], nand1_out, trash);
  
  clflush(nand2_out);
  mfence();  
  trash = nand_fan_gate(in2Copies[0], in2Copies[1], nand2_out, trash);
  
  clflush(out);
  mfence();
  
  trash = nand_fan_gate(nand1_out, nand2_out, out, trash);
  
  return_temporary_memory(nand2_out);
  return_temporary_memory(nand1_out);
  
  return_temporary_memory(in2Copies[1]);
  return_temporary_memory(in2Copies[0]);
  
  return_temporary_memory(in1Copies[1]);
  return_temporary_memory(in1Copies[0]);
  
  
  return trash;
  
  
}

uintptr_t test_or_from_nand_gate(uintptr_t trash) {
  {
  
  printf("OR FROM NAND GATE: \n");
  
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  printf("1 1 : %d\n", output_read);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}

uintptr_t test_or_from_nand_gate_accuracy(uintptr_t trash, uintptr_t iteration) {
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
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
  
  trash = or_from_nand_impl(input1, input2, output, trash);
  
  trash = read_addr(output, &output_read, trash);
  
  if (output_read == 1)
    correct++;
  else
    incorrect++;
  }
  printf("OR FROM NAND Accuracy : %ld / %ld\n", correct, correct + incorrect);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}