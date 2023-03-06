#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t mux_impl(uintptr_t a, uintptr_t b, uintptr_t sel, uintptr_t output, uintptr_t trash) {
  
  
  
  

  a = a | (a == 0xbaaaaad);
  b = b | (b == 0xbaaaaad);
  sel = sel | (sel == 0xbaaaaad);
  output = output | (output == 0xbaaaaad);
  trash = trash | (trash == 0xbaaaaad);
  
  mfence();
  lfence();
  uintptr_t selCopies[2];
  selCopies[0] = get_temporary_memory();
  selCopies[1] = get_temporary_memory();
  
  uintptr_t not_out = get_temporary_memory();
  uintptr_t and_1_out = get_temporary_memory();
  uintptr_t and_2_out = get_temporary_memory();
  
  clflush(selCopies[0]);
  clflush(selCopies[1]);
  mfence();
  lfence();
  
  trash = fan_gate(sel, selCopies[0], selCopies[1], trash);
  
  clflush(not_out);
  mfence();
  lfence();
  trash = not_fan_gate(selCopies[0], not_out, trash);
  
  clflush(and_1_out);
  mfence();
  lfence();
  trash = and_fan_gate(a, not_out, and_1_out, trash);

  
  clflush(and_2_out);
  mfence();
  lfence();
  trash = and_fan_gate(b, selCopies[1], and_2_out, trash);
  
  mfence();
  lfence();
  
  trash = or_from_nor_not(and_1_out, and_2_out, output, trash);

  mfence();
  lfence();
  
  return_temporary_memory(and_2_out);
  return_temporary_memory(and_1_out);
  return_temporary_memory(not_out);
  
  return_temporary_memory(selCopies[1]);
  return_temporary_memory(selCopies[0]);
  
  
  return trash;
}

uintptr_t test_mux(uintptr_t trash) {
{
  
  printf("MUX: \n");
  
  uintptr_t out_read;

  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t input3 = get_temporary_memory();
  uintptr_t out = get_temporary_memory();
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 0, b = 0, sel = 0 - output: %d\n", out_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);

  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 1, b = 0, sel = 0 - output: %d\n", out_read);
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 0, b = 1, sel = 0 - output: %d\n", out_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 1, b = 1, sel = 0 - output: %d\n", out_read);
  

  
  mfence();
  lfence();
  
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 0, b = 0, sel = 1 - output: %d\n", out_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 1, b = 0, sel = 1 - output: %d\n", out_read);
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 0, b = 1, sel = 1 - output: %d\n", out_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  printf("a = 1, b = 1, sel = 1 - output: %d\n", out_read);
  
  return_temporary_memory(out);
  return_temporary_memory(input3);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
return trash;
}

uintptr_t test_mux_accuracy(uintptr_t trash, uintptr_t iteration) {

  
  uintptr_t correct = 0;
  uintptr_t incorrect = 0;
  
  uintptr_t out_read;

  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t input3 = get_temporary_memory();
  uintptr_t out = get_temporary_memory();
  
  for (uintptr_t i = 0; i < iteration; i++) {
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);

  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 1)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 1)
    correct++;
  else
    incorrect++;
  

  
  mfence();
  lfence();
  
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 1)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(input3);
  clflush(out);
  
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  trash = FORCE_READ(input3, trash);
  
  trash = mux_impl(input1, input2, input3, out, trash);
  
  trash = read_addr(out, &out_read, trash);
  
  if (out_read == 1)
    correct++;
  else
    incorrect++;
  
  }
  printf("MUX Accuracy : %ld / %ld\n", correct, correct + incorrect);
  
  return_temporary_memory(out);
  return_temporary_memory(input3);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  
return trash;
}