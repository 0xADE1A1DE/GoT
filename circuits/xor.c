#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t  xor_impl(uintptr_t in1, uintptr_t in2, uintptr_t out, uintptr_t trash) {
  
  uintptr_t in1Copies[2];
  uintptr_t in2Copies[2];
  
  uintptr_t nand_out = get_temporary_memory();
  uintptr_t or_out = get_temporary_memory();
  
  in1Copies[0] = get_temporary_memory();
  in1Copies[1] = get_temporary_memory();
  
  in2Copies[0] = get_temporary_memory();
  in2Copies[1] = get_temporary_memory();
  
  
  

  mfence();
  lfence();
  
  clflush(in1Copies[0]);
  clflush(in1Copies[1]);
  mfence();
  lfence();
  
  trash = fan_gate(in1, in1Copies[0], in1Copies[1], trash);
  
  

  clflush(in2Copies[0]);
  clflush(in2Copies[1]);
  mfence();
  lfence(); 

  trash = fan_gate(in2, in2Copies[0], in2Copies[1], trash);
  
  
  
  clflush(nand_out);
  mfence();
  lfence();
  
  trash = nand_fan_gate(in1Copies[0], in2Copies[0], nand_out, trash);
  
  
  
  clflush(or_out);
  mfence();
  lfence();
  
  trash = or_from_nor_not(in1Copies[1], in2Copies[1], or_out, trash);
  
  
  
  clflush(out);
  mfence();
  lfence();
  
  trash = and_fan_gate(nand_out, or_out, out, trash);
  
  
  
  return_temporary_memory(or_out);
  return_temporary_memory(nand_out);
  
  return_temporary_memory(in2Copies[1]);
  return_temporary_memory(in2Copies[0]);
  
  return_temporary_memory(in1Copies[1]);
  return_temporary_memory(in1Copies[0]);
  
  return trash;
  
  
}

uintptr_t xor_impl_2(uintptr_t in1, uintptr_t in2, uintptr_t out1, uintptr_t out2, uintptr_t trash) {
  
  uintptr_t in1Copies[2];
  uintptr_t in2Copies[2];
  
  uintptr_t nand_out = get_temporary_memory();
  uintptr_t or_out = get_temporary_memory();
  
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
  
  clflush(nand_out);
  mfence();
  
  trash = nand_fan_gate(in1Copies[0], in2Copies[0], nand_out, trash);
  
  clflush(or_out);
  mfence();
  
  trash = or_from_nor_not(in1Copies[1], in2Copies[1], or_out, trash);
  
  clflush(out1);
  clflush(out2);
  mfence();
  
  trash = and_fan_gate_2(nand_out, or_out, out1, out2, trash);
  
  return_temporary_memory(or_out);
  return_temporary_memory(nand_out);
  
  return_temporary_memory(in2Copies[1]);
  return_temporary_memory(in2Copies[0]);
  
  return_temporary_memory(in1Copies[1]);
  return_temporary_memory(in1Copies[0]);
  
  
  return trash;
}


uintptr_t  test_xor(uintptr_t trash) {
{
  
  printf("XOR GATE: \n");
  
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
  
  trash = xor_impl(input1, input2, output, trash);

  
  
  
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

  mfence();
  lfence();
  
  trash = xor_impl(input1, input2, output, trash);

  
  
  
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

  mfence();
  lfence();
  
  trash = xor_impl(input1, input2, output, trash);

  
  
  
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

  mfence();
  lfence();
  
  trash = xor_impl(input1, input2, output, trash);

  
  
  
  trash = read_addr(output, &output_read, trash);

  
  
  
  printf("1 1 : %d\n", output_read);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
return trash;
}

uintptr_t  test_xor_accuracy(uintptr_t trash, uintptr_t iteration) {
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
  
  trash = xor_impl(input1, input2, output, trash);
  
  


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
  

  mfence();
  lfence();
  
  trash = xor_impl(input1, input2, output, trash);
  
  


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
  

  mfence();
  lfence();
  
  trash = xor_impl(input1, input2, output, trash);
  
  


  
  trash = read_addr(output, &output_read, trash);


  if (output_read == 1)
    correct++;
  else
    incorrect++;
  
  
  
  clflush(input1);
  clflush(input2);
  clflush(output);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);

  
  

  mfence();
  lfence();
  
  trash = xor_impl(input1, input2, output, trash);
  
  


  trash = read_addr(output, &output_read, trash);

  
  if (output_read == 0)
    correct++;
  else
    incorrect++;
  
  }
  printf("XOR Accuracy : %ld / %ld\n", correct, correct + incorrect);
  
  return_temporary_memory(output);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
return trash;
}