#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include <sys/mman.h>
#include <stddef.h>

#include "mm.h"

extern uintptr_t temporary_memory[TEMP_MEMORY_SIZE];
extern void *memory;

uintptr_t full_adder_impl(volatile uintptr_t a, volatile uintptr_t b, volatile uintptr_t c, volatile uintptr_t sum, volatile uintptr_t carry, volatile uintptr_t trash)
{
  fetch_address(&a, L1);
  fetch_address(&b, L1);
  fetch_address(&c, L1);
  fetch_address(&sum, L1);
  fetch_address(&carry, L1);
  fetch_address(&trash, L1);
  
  mfence();
  lfence();
  
  volatile uintptr_t ha1_sum = get_temporary_memory();
  volatile uintptr_t ha1_carry = get_temporary_memory();
  
  volatile uintptr_t ha2_carry = get_temporary_memory();

  clflush(ha1_sum);
  clflush(ha1_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(a, b, ha1_sum, ha1_carry, trash);
  
  clflush(sum);
  clflush(ha2_carry);
  
  
  
  
  trash = half_adder_impl(c, ha1_sum, sum, ha2_carry, trash);
  
  clflush(carry);
  
  mfence();
  lfence();
  
  trash = or_from_nor_not(ha1_carry, ha2_carry, carry, trash);
  
  mfence();
  lfence();
  
  return_temporary_memory(ha2_carry);
  return_temporary_memory(ha1_carry);
  return_temporary_memory(ha1_sum);

  return trash;
}

uintptr_t test_full_adder(uintptr_t trash)
{
  {
    
    printf("FULL ADDER: \n");

    uintptr_t sum_read;
    uintptr_t carry_read;

    uintptr_t input1 = get_temporary_memory();
    uintptr_t input2 = get_temporary_memory();
    uintptr_t input3 = get_temporary_memory();
    uintptr_t sum = get_temporary_memory();
    uintptr_t carry = get_temporary_memory();

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    mfence();
    lfence();

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("0 0 0 - sum: %d, carry: %d\n", sum_read, carry_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    mfence();
    lfence();

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("1 0 0 - sum: %d, carry: %d\n", sum_read, carry_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    mfence();
    lfence();

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("0 1 0- sum: %d, carry: %d\n", sum_read, carry_read);
    

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    mfence();
    lfence();

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("1 1 0- sum: %d, carry: %d\n", sum_read, carry_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("0 0 1 - sum: %d, carry: %d\n", sum_read, carry_read);
    

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    mfence();
    lfence();

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("1 0 1 - sum: %d, carry: %d\n", sum_read, carry_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    mfence();
    lfence();

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("0 1 1 - sum: %d, carry: %d\n", sum_read, carry_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    mfence();
    lfence();

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    mfence();
    lfence();

    printf("1 1 1 - sum: %d, carry: %d\n", sum_read, carry_read);

    return_temporary_memory(carry);
    return_temporary_memory(sum);
    return_temporary_memory(input3);
    return_temporary_memory(input2);
    return_temporary_memory(input1);
  }
  return trash;
}

#define TARGET_SUCCESS_COUNTS 10

uintptr_t test_full_adder_accuracy(uintptr_t trash, uintptr_t iteration)
{
  uintptr_t correct;
  uintptr_t incorrect;
  uintptr_t target_accuracy_success = 0;

  while (true) {
  correct = 0;
  incorrect = 0;

  uintptr_t sum_read;
  uintptr_t carry_read;

  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t input3 = get_temporary_memory();
  uintptr_t sum = get_temporary_memory();
  uintptr_t carry = get_temporary_memory();

  for (uintptr_t i = 0; i < iteration; i++)
  {

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

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
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

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
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

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
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

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

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input3, trash);

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

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
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input3, trash);

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

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

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

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

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(sum);
    clflush(carry);

    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    trash = full_adder_impl(input1, input2, input3, sum, carry, trash);

    trash = read_addr(sum, &sum_read, trash);
    trash = read_addr(carry, &carry_read, trash);

    if (sum_read == 1)
      correct++;
    else
      incorrect++;

    if (carry_read == 1)
      correct++;
    else
      incorrect++;
  }
  printf("Full Adder Accuracy : %d / %d\n", correct, correct + incorrect);
  return_temporary_memory(carry);
  return_temporary_memory(sum);

  return_temporary_memory(input3);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  const double accuracy = (double) correct / (double) (incorrect + correct);

  const double TARGET_ACCURACY = 0.9;
  if (accuracy < TARGET_ACCURACY) {
    printf("accuracy fell short, randomising temp memory!\n");
    
    target_accuracy_success = 0;
    randomise_temp_memory();
      
      
      
      
      
      
      
  } else {
    target_accuracy_success++;
    if (target_accuracy_success >= TARGET_SUCCESS_COUNTS) {
      printf("target accuract reached! continuing test\n");
      printf("temp memory: {");
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++) {
        printf("%ld, ", (temporary_memory[i] - (uintptr_t)memory) / CACHE_LINE_SIZE);
      }
      printf("}\n");
      break;
    } else {
      printf("Target accuracy reached, doing %ld more time(s) with the same configuration\n", 
      TARGET_SUCCESS_COUNTS - target_accuracy_success);
    }


  }

  }
  return trash;
}