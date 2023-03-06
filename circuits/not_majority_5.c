#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t  not_majority_5_impl(uintptr_t in1, uintptr_t in2, uintptr_t in3, 
                               uintptr_t in4, uintptr_t in5, uintptr_t out, bool wet_run, uintptr_t trash)
{


    
    
    
    









  
  for (int i = 0; i < 128; i++)
  {
    asm volatile("");
  }
  
  fetch_address(&in1, L1);
  fetch_address(&in2, L1);
  fetch_address(&in3, L1);
  fetch_address(&in4, L1);
  fetch_address(&in5, L1);
  fetch_address(&out, L1);
  fetch_address(&wet_run, L1);
  fetch_address(&trash, L1);
  
  
  
  
  
  
  
  
  
  trash = FORCE_READ(in1 - 128, trash);
  trash = FORCE_READ(in2 - 128, trash);
  trash = FORCE_READ(in3 - 128, trash);
  trash = FORCE_READ(in4 - 128, trash);
  trash = FORCE_READ(in5 - 128, trash);
  trash = FORCE_READ(out - 128, trash);
  

  mfence();
  lfence();

  register uintptr_t val1 = FORCE_READ(in1, trash);
  register uintptr_t val2 = FORCE_READ(in2, trash);
  register uintptr_t val3 = FORCE_READ(in3, trash);
  register uintptr_t val4 = FORCE_READ(in4, trash);
  register uintptr_t val5 = FORCE_READ(in5, trash);

  volatile uintptr_t new_trash1 = val1 + val2 + val3;

  volatile uintptr_t new_trash2 = val1 + val3 + val4;
  volatile uintptr_t new_trash3 = val1 + val4 + val5;

  volatile uintptr_t new_trash4 = val2 + val3 + val4;
  volatile uintptr_t new_trash5 = val2 + val3 + val5;
  
  volatile uintptr_t new_trash6 = val3 + val4 + val5;
  
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

  if (wet_run == (new_trash4 != 0xbaaaaad))
  {
    return new_trash4;
  }

  if (wet_run == (new_trash5 != 0xbaaaaad))
  {
    return new_trash5;
  }

  if (wet_run == (new_trash6 != 0xbaaaaad))
  {
    return new_trash6;
  }

  
  asm volatile("");
  if (!wet_run)
    return new_trash1;

  
  
  
  
  float always_zero = (float)(wet_run - 1);
  #pragma GCC unroll 300
  for (int i = 0; i < SLOW_PARAM - 14; i++) {  always_zero *= always_zero; }

  trash += *(uintptr_t *)(out | (always_zero != 0));
  return trash + new_trash1 + new_trash2 + new_trash3 + new_trash4 + new_trash5 + new_trash6;
}

uintptr_t not_majority_5_gate(uintptr_t in1, uintptr_t in2, uintptr_t in3,
                              uintptr_t in4, uintptr_t in5, uintptr_t out, uintptr_t trash)
{
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 1;
  #endif
  gate_metadata __attribute__((aligned(64))) fake = {0};
  
  
  

  trash = not_majority_5_impl(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_5_impl(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_5_impl(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_5_impl(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = not_majority_5_impl(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  
  trash = not_majority_5_impl(in1, in2, in3, in4, in5, out, true, trash);

  
  
  

  return trash;
}

uintptr_t test_not_majority_5(uintptr_t trash)
{
  
    
    printf("NOT MAJORITY 5 GATE: \n");

    uintptr_t output1_read;

    uintptr_t input1 = get_temporary_memory();
    uintptr_t input2 = get_temporary_memory();
    uintptr_t input3 = get_temporary_memory();
    uintptr_t input4 = get_temporary_memory();
    uintptr_t input5 = get_temporary_memory();
    uintptr_t output1 = get_temporary_memory();

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 0 0 0: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 0 0 0 0: %d\n", output1_read);
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 1 0 0 0: %d\n", output1_read);
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 1 0 0: %d\n", output1_read);
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input4, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 0 1 0: %d\n", output1_read);
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input5, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 0 0 1: %d\n", output1_read);
    

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 1 0 0 0: %d\n", output1_read);
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 1 1 0 0: %d\n", output1_read);
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input3, trash);
    trash = FORCE_READ(input4, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 1 1 0: %d\n", output1_read);
    

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input4, trash);
    trash = FORCE_READ(input5, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 0 1 1: %d\n", output1_read);
    

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 1 1 1 0: %d\n", output1_read);
    


    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);
    trash = FORCE_READ(input4, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 1 1 1 0: %d\n", output1_read);
    
    
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input3, trash);
    trash = FORCE_READ(input4, trash);
    trash = FORCE_READ(input5, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 0 1 1 1: %d\n", output1_read);
    
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);
    trash = FORCE_READ(input4, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 1 1 1 0: %d\n", output1_read);
    
    
    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);
    trash = FORCE_READ(input4, trash);
    trash = FORCE_READ(input5, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 1 1 1 1: %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input1);
    clflush(input2);
    clflush(input3);
    clflush(input4);
    clflush(input5);
    clflush(output1);
    mfence();
    lfence();

    trash = FORCE_READ(input1, trash);
    trash = FORCE_READ(input2, trash);
    trash = FORCE_READ(input3, trash);
    trash = FORCE_READ(input4, trash);
    trash = FORCE_READ(input5, trash);

    mfence();
    lfence();

    trash = not_majority_5_gate(input1, input2, input3, input4, input5, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("1 1 1 1 1: %d\n", output1_read);



    return_temporary_memory(output1);
    return_temporary_memory(input5);
    return_temporary_memory(input4);
    return_temporary_memory(input3);
    return_temporary_memory(input2);
    return_temporary_memory(input1);
  
  return trash;
}

uintptr_t test_not_majority_5_accuracy(uintptr_t trash, uintptr_t iteration,
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
  
  printf("Not Majority 5 Accuracy : %ld / %ld ", correct, correct + incorrect);
  print_state(state);
  return_temporary_memory(input3);
  return_temporary_memory(input2);
  return_temporary_memory(input1);
  return_temporary_memory(output1);

  return trash;
}

