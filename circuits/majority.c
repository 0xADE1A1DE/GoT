#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t majority_gate(uintptr_t input1, uintptr_t input2, uintptr_t input3, uintptr_t output, uintptr_t trash) {
  uintptr_t not_majority_out = get_temporary_memory();

  clflush(not_majority_out);
  mfence();
  lfence();

  trash = not_majority_gate(input1, input2, input3, not_majority_out, trash);

  mfence();
  lfence();

  trash = not_fan_gate(not_majority_out, output, trash);

  mfence();
  lfence();

  return_temporary_memory(not_majority_out);

  return trash;
}

uintptr_t majority_gate_5(uintptr_t input1, uintptr_t input2, uintptr_t input3, uintptr_t input4, uintptr_t input5, uintptr_t output, uintptr_t trash) {
  uintptr_t not_majority_out = get_temporary_memory();

  clflush(not_majority_out);
  mfence();
  lfence();

  trash = not_majority_5_gate(input1, input2, input3, input4, input5, not_majority_out, trash);

  mfence();
  lfence();

  trash = not_fan_gate(not_majority_out, output, trash);

  mfence();
  lfence();

  return_temporary_memory(not_majority_out);

  return trash;
}

uintptr_t test_majority_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state)
{

  
  uintptr_t correct = 0;
  uintptr_t incorrect = 0;

  uintptr_t output1_read;

  uintptr_t input1 = get_temporary_memory();
  uintptr_t input2 = get_temporary_memory();
  uintptr_t input3 = get_temporary_memory();
  uintptr_t output1 = get_temporary_memory();

  for (uintptr_t i = 0; i < iteration; i++)
  {
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 0)
      correct++;
    else
      incorrect++;
   
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    
    fetch_address(input1, state);
    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 0)
      correct++;
    else
      incorrect++;

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    
    fetch_address(input2, state);
    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 0)
      correct++;
    else
      incorrect++;

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    
    fetch_address(input3, state);
    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 0)
      correct++;
    else
      incorrect++;

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    
    
    fetch_address(input1, state);
    fetch_address(input2, state);
    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 1)
      correct++;
    else
      incorrect++;

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    
    
    fetch_address(input1, state);
    fetch_address(input3, state);
    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 1)
      correct++;
    else
      incorrect++;

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    
    
    fetch_address(input2, state);
    fetch_address(input3, state);


    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 1)
      correct++;
    else
      incorrect++;

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    clflush(output1);
    mfence();
    lfence();

    
    
    

    fetch_address(input1, state);
    fetch_address(input2, state);
    fetch_address(input3, state);

    mfence();
    lfence();

    trash = majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 1)
      correct++;
    else
      incorrect++;

  }
  
  printf("Majority Accuracy : %ld / %ld ", correct, correct + incorrect);
  print_state(state);
  return_temporary_memory(output1);
  return_temporary_memory(input3);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  return trash;
}

