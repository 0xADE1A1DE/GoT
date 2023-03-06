#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t dmux_impl(uintptr_t in, uintptr_t sel,
                    uintptr_t a, uintptr_t b, uintptr_t trash) {
  uintptr_t selCopies[2];
  uintptr_t inCopies[2];
  
  selCopies[0] = get_temporary_memory();
  selCopies[1] = get_temporary_memory();
  
  inCopies[0] = get_temporary_memory();
  inCopies[1] = get_temporary_memory();
  
  uintptr_t not_sel_out = get_temporary_memory();
  
  clflush(selCopies[0]);
  clflush(selCopies[1]);
  mfence();
  lfence();
  trash = fan_gate(sel, selCopies[0], selCopies[1], trash);
  
  clflush(not_sel_out);
  mfence();
  lfence();
  trash = not_fan_gate(selCopies[0], not_sel_out, trash);
  
  clflush(inCopies[0]);
  clflush(inCopies[1]);
  mfence();
  lfence();
  trash = fan_gate(in, inCopies[0], inCopies[1], trash);
  mfence();
  lfence();
  
  trash = and_fan_gate(inCopies[0], not_sel_out, a, trash);
  mfence();
  lfence();
  trash = and_fan_gate(inCopies[1], selCopies[1], b, trash);
  mfence();
  lfence();
  
  return_temporary_memory(not_sel_out);
  
  return_temporary_memory(inCopies[1]);
  return_temporary_memory(inCopies[0]);
  
  return_temporary_memory(selCopies[1]);
  return_temporary_memory(selCopies[0]);
  
  
  return trash;
}

uintptr_t test_dmux(uintptr_t trash) {
{  
  
  printf("DMUX: \n");
  
  uintptr_t output_1_read;
  uintptr_t output_2_read;
  
  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t output1 = get_temporary_memory();
  uintptr_t output2 = get_temporary_memory();
  
  
  
  
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  printf("in = 0, sel = 0 - output1 = %d, output2 = %d\n", output_1_read, output_2_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  printf("in = 1, sel = 0 - output1 = %d, output2 = %d\n", output_1_read, output_2_read);
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  printf("in = 0, sel = 1 - output1 = %d, output2 = %d\n", output_1_read, output_2_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
  printf("in = 1, sel = 1 - output1 = %d, output2 = %d\n", output_1_read, output_2_read);
  
  return_temporary_memory(output2);
  return_temporary_memory(output1);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
return trash;
}

uintptr_t test_dmux_accuracy(uintptr_t trash, uintptr_t iteration) {
{  
  

  uintptr_t correct = 0;
  uintptr_t incorrect = 0;
  
  uintptr_t output_1_read;
  uintptr_t output_2_read;
  
  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t output1 = get_temporary_memory();
  uintptr_t output2 = get_temporary_memory();
  
  
  
  
  
  
  
  for (uintptr_t i = 0 ; i < iteration; i++) {
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(output1);
  clflush(output2);
  
  mfence();
  lfence();
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
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
  
  trash = FORCE_READ(input1, trash);
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
    if (output_1_read == 1)
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
  
  trash = FORCE_READ(input2, trash);
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
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
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  trash = dmux_impl(input1, input2, output1, output2, trash);
  
  trash = read_addr(output1, &output_1_read, trash);
  trash = read_addr(output2, &output_2_read, trash);
  
    if (output_1_read == 0)
      correct++;
    else
      incorrect++;

    if (output_2_read == 1)
      correct++;
    else
      incorrect++;
  }
  printf("DMUX Accuracy : %ld / %ld\n", correct, correct + incorrect);
  
  return_temporary_memory(output2);
  return_temporary_memory(output1);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
return trash;
}