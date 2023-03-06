#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t  not_fan_gate_impl(volatile uintptr_t in, volatile uintptr_t out, volatile bool wet_run, volatile uintptr_t trash)
{



    
    
    
    

  
  
  
  
  
  









  
  for (int i = 0; i < 128; i++)
  {
    asm volatile("");
  }
  
  
  
  
  
  
  in = in | (in == 0xbaaaaad);
  out = out | (out == 0xbaaaaad);
  wet_run = wet_run | (wet_run == 0xbaaaaad);
  trash = trash | (trash == 0xbaaaaad);
  
  
  
  
  trash = FORCE_READ(in - 128, trash);
  trash = FORCE_READ(out - 128, trash);
  

  mfence();
  lfence();

  uintptr_t new_trash = FORCE_READ(in, trash);
  if (wet_run == (new_trash != 0xbaaaaad))
  {
    return new_trash;
  }

  
  asm volatile("");
  if (!wet_run)
    return new_trash;

  
  
  
  
  
  volatile float always_zero = (float)(wet_run - 1);
  #pragma GCC unroll 300
  for (int i = 0; i < SLOW_PARAM - 3; i++) { always_zero *= always_zero; }

  trash += *(uintptr_t *)((out) | (always_zero != 0));
  return trash + new_trash;
}

uintptr_t not_fan_gate(uintptr_t in, uintptr_t out, uintptr_t trash)
{
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 1;
  #endif
  gate_metadata __attribute__((aligned(64))) fake = {0};
  
  
  

  trash = not_fan_gate_impl(&fake, &fake, false, trash);
  trash = not_fan_gate_impl(&fake, &fake, false, trash);
  trash = not_fan_gate_impl(&fake, &fake, false, trash);
  trash = not_fan_gate_impl(&fake, &fake, false, trash);
  trash = not_fan_gate_impl(in, out, true, trash);

  
  
  

  return trash;
}

uintptr_t test_not(uintptr_t trash)
{
  {
    
    printf("NOT FAN GATE: \n");

    uintptr_t output1_read;

    uintptr_t input = get_temporary_memory();
    uintptr_t output1 = get_temporary_memory();

    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    mfence();
    lfence();

    trash = not_fan_gate(input, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 : %d\n", output1_read);

    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);

    mfence();
    lfence();

    trash = FORCE_READ(input, trash);

    trash = not_fan_gate(input, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    printf("0 : %d\n", output1_read);

    return_temporary_memory(output1);
    return_temporary_memory(input);
  }
  return trash;
}

uintptr_t test_not_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state)
{

  
  uintptr_t correct = 0;
  uintptr_t incorrect = 0;

  uintptr_t output1_read;

  uintptr_t input = get_temporary_memory();
  uintptr_t output1 = get_temporary_memory();

  for (uintptr_t i = 0; i < iteration; i++)
  {
    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    mfence();
    lfence();

    trash = not_fan_gate(input, output1, trash);

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 1)
      correct++;
    else
      incorrect++;
    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);

    mfence();
    lfence();

    fetch_address(input, state);

    trash = not_fan_gate(input, output1, trash);

    mfence();
    lfence();

    trash = read_addr(output1, &output1_read, trash);

    if (output1_read == 0)
      correct++;
    else
      incorrect++;
  }
  
  printf("Not Accuracy : %ld / %ld ", correct, correct + incorrect);
  print_state(state);
  return_temporary_memory(output1);
  return_temporary_memory(input);

  return trash;
}