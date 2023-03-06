#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t  not_majority_impl(uintptr_t in1, uintptr_t in2, uintptr_t in3, uintptr_t out, bool wet_run, uintptr_t trash)
{


    
    
    
    










  for (int i = 0; i < 128; i++)
  {
    asm volatile("");
  }
  
  fetch_address(&in1, L1);
  fetch_address(&in2, L1);
  fetch_address(&in3, L1);
  fetch_address(&out, L1);
  fetch_address(&wet_run, L1);
  fetch_address(&trash, L1);
  
  
  
  
  
  
  trash = FORCE_READ(in1 - 128, trash);
  trash = FORCE_READ(in2 - 128, trash);
  trash = FORCE_READ(in3 - 128, trash);
  trash = FORCE_READ(out - 128, trash);

  mfence();
  lfence();
  

  register uintptr_t val1 = FORCE_READ_PLAIN(in1);
  register uintptr_t val2 = FORCE_READ_PLAIN(in2);
  register uintptr_t val3 = FORCE_READ_PLAIN(in3);

  volatile uintptr_t new_trash1 = val1 + val2;
  volatile uintptr_t new_trash2 = val1 + val3;
  volatile uintptr_t new_trash3 = val2 + val3;
  
  if (wet_run == (new_trash1 != 0xbaaaaad))
  {
    return new_trash1;
  }

  if (wet_run == (new_trash2 != 0xbaaaaad))
  {
    return new_trash2;
  }

  if (wet_run == (new_trash3 != 0xbaaaaad))
  {
    return new_trash3;
  }




  
  asm volatile("");
  if (!wet_run)
    return new_trash1;

  
  
  
  float always_zero = (float)(wet_run - 1);
  #pragma GCC unroll 300
  for (int i = 0; i < SLOW_PARAM - 3; i++) { always_zero *= always_zero; }

  trash = *(uintptr_t *)((out) | (always_zero != 0));
  return trash + new_trash1 + new_trash2 + new_trash3;
}

uintptr_t not_majority_gate(uintptr_t in1, uintptr_t in2, uintptr_t in3, 
                            uintptr_t out, uintptr_t trash)
{
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 1;
  #endif
  
  gate_metadata __attribute__((aligned(64))) fake = {0};
  
  
  

  trash = not_majority_impl(&fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_impl(&fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_impl(&fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_impl(&fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_impl(in1, in2, in3, out, true, trash);

  
  
  

  return trash;
}  

uintptr_t test_not_majority(uintptr_t trash)
{
  
    
    printf("NOT MAJORITY GATE: \n");

    uintptr_t output1_read;

    uintptr_t input1 = get_temporary_memory();
    uintptr_t input2 = get_temporary_memory();
    uintptr_t input3 = get_temporary_memory();
    uintptr_t output1 = get_temporary_memory();

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 0: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);

    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 0 0: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);

    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 1 0: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 1: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);

    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 1 0: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input3, trash);


    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 0 1: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 1 1: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 1 1: %d\n", output1_read);

    return_temporary_memory(output1);
    return_temporary_memory(input3);
    return_temporary_memory(input2);
    return_temporary_memory(input1);
  
  return trash;
}

uintptr_t test_not_majority_accuracy(uintptr_t trash, uintptr_t iteration,
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

    trash = not_majority_gate(input1, input2, input3, output1, trash);

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
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

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
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

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

    
    fetch_address(input3, state);
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

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
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

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
    fetch_address(input3, state);
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

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
    fetch_address(input3, state);
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

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
    fetch_address(input3, state);
    mfence();
    lfence();

    trash = not_majority_gate(input1, input2, input3, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 0)
      correct++;
    else
      incorrect++;

  }
  
  printf("Not Majority Accuracy : %ld / %ld ", correct, correct + incorrect);
  print_state(state);
  return_temporary_memory(output1);
  return_temporary_memory(input3);
  return_temporary_memory(input2);
  return_temporary_memory(input1);

  return trash;
}

