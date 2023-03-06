#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"








uintptr_t half_adder_impl(volatile uintptr_t a, volatile uintptr_t b, volatile uintptr_t sum, volatile uintptr_t carry, volatile uintptr_t trash) {
  fetch_address(&a, L1);
  fetch_address(&b, L1);
  fetch_address(&sum, L1);
  fetch_address(&carry, L1);
  fetch_address(&trash, L1);
  
  
  
  
  
  

  mfence();
  lfence();
 
  
  
  
  uintptr_t a_copy_1 = get_temporary_memory();
  uintptr_t a_copy_2 = get_temporary_memory();
  
  uintptr_t b_copy_1 = get_temporary_memory();
  uintptr_t b_copy_2 = get_temporary_memory();
  
  
  
  
  
  
  
  volatile uintptr_t or_output = get_temporary_memory();
  volatile uintptr_t and_output = get_temporary_memory();
  
  volatile uintptr_t not_output = get_temporary_memory();
  
  mfence();
  lfence();
  
  clflush(a_copy_1);
  clflush(a_copy_2);
  
  mfence();
  lfence();
  
  trash = fan_gate(a, a_copy_1, a_copy_2, trash);

  clflush(b_copy_1);
  clflush(b_copy_2);
  

  mfence();
  lfence();

  trash = fan_gate(b, b_copy_1, b_copy_2, trash);

  clflush(carry);
  clflush(and_output);

  mfence();
  lfence();
  trash = and_fan_gate_2(a_copy_1, b_copy_1,
                         and_output, carry, trash);


  clflush(or_output);

  mfence();
  lfence();
  trash = or_from_nor_not(a_copy_2, b_copy_2, or_output, trash); 
  
  clflush(not_output);
  
  mfence();
  lfence();
  trash = not_fan_gate(and_output, not_output, trash);
 
  
  clflush(sum);
  
  mfence();
  lfence();
  trash = and_fan_gate(or_output, not_output, sum, trash);
  
  mfence();
  lfence();


  return_temporary_memory(not_output);

  return_temporary_memory(and_output);
  
  return_temporary_memory(or_output);
  
  return_temporary_memory(b_copy_2);
  return_temporary_memory(b_copy_1);

  return_temporary_memory(a_copy_2);
  return_temporary_memory(a_copy_1);
  
  return trash;
  
}




  


  


  




  














  







  







  





  


uintptr_t test_half_adder(uintptr_t trash) {
{
  
  printf("HALF ADDER: \n");
  
  uintptr_t sum_read;
  uintptr_t carry_read;
  
  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t sum = get_temporary_memory();
  uintptr_t carry = get_temporary_memory();
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
  printf("0 0 - sum: %d, carry: %d\n", sum_read, carry_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
  printf("1 0 - sum: %d, carry: %d\n", sum_read, carry_read);
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
  printf("0 1 - sum: %d, carry: %d\n", sum_read, carry_read);
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
  printf("1 1 - sum: %d, carry: %d\n", sum_read, carry_read);
  
  return_temporary_memory(carry);
  return_temporary_memory(sum);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}

uintptr_t test_half_adder_accuracy(uintptr_t trash, uintptr_t iteration) {
{
  
  
  
  
  
  
  uintptr_t correct = 0;
  uintptr_t incorrect = 0;
  
  uintptr_t sum_read;
  uintptr_t carry_read;
  
  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t sum = get_temporary_memory();
  uintptr_t carry = get_temporary_memory();
  
  for (uintptr_t i = 0; i < iteration; i++) {
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
    if (sum_read == 0)
      correct++;
    else
      incorrect++;

    if (carry_read == 0)
      correct++;
    else
      incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);

  mfence();
  lfence();
  
  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
    if (sum_read == 1)
      correct++;
    else
      incorrect++;

    if (carry_read == 0)
      correct++;
    else
      incorrect++;
  
  
  
  mfence();
  lfence();

  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input2, trash);

  mfence();
  lfence();
  
  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
  if (sum_read == 1)
    correct++;
  else
    incorrect++;

  if (carry_read == 0)
    correct++;
  else
    incorrect++;
  
  
  
  mfence();
  lfence();
  
  clflush(input1);
  clflush(input2);
  clflush(sum);
  clflush(carry);
  
  
  mfence();
  lfence();
  
  trash = FORCE_READ(input1, trash);
  trash = FORCE_READ(input2, trash);
  
  mfence();
  lfence();

  trash = half_adder_impl(input1, input2, sum, carry, trash);
  
  trash = read_addr(sum, &sum_read, trash);
  trash = read_addr(carry, &carry_read, trash);
  
    if (sum_read == 0)
      correct++;
    else
      incorrect++;

    if (carry_read == 1)
      correct++;
    else
      incorrect++;
  
  }
  
  printf("Half Adder Accuracy : %ld / %ld\n", correct, correct + incorrect);
  
  return_temporary_memory(carry);
  return_temporary_memory(sum);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  }
  return trash;
}