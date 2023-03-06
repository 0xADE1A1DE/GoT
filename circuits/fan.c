#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t fan_gate_impl (uintptr_t in, uintptr_t out1, uintptr_t out2, bool wet_run, uintptr_t trash)
{
  
  
  
  

  
  
  
  
  

  in = in | (in == 0xbaaaaad);
  out1 = out1 | (out1 == 0xbaaaaad);
  out2 = out2 | (out2 == 0xbaaaaad);
  wet_run = wet_run | (wet_run == 0xbaaaaad);
  trash = trash | (trash == 0xbaaaaad);
  
  
  
  
  
  trash = FORCE_READ(in - 128, trash);
  trash = FORCE_READ(out1 - 128, trash);
  trash = FORCE_READ(out2 - 128, trash);
  

  mfence();
  lfence();
  
  
  const double start = DBL_MIN;
  const double denormal_result = DBL_MIN / 2;

  double divide_by = wet_run + 1;
  double result = start / divide_by;

  if (result == denormal_result)
    return trash;

  
  asm volatile("");
  if (!wet_run)
    return trash;

  
  
  

  
  trash = *(uintptr_t *)(in); 

  
  
  

  uintptr_t sum = 0;
  
  

  sum += *(uintptr_t *)(out1 + trash);
  sum += *(uintptr_t *)(out2 + trash);

  return trash + sum;
}

uintptr_t fan_gate_impl_3(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, 
                          bool wet_run, uintptr_t trash)
{
  
  
  
  
  
  
  
  
  
  
  

    in = in | (in == 0xbaaaaad);
    out1 = out1 | (out1 == 0xbaaaaad);
    out2 = out2 | (out2 == 0xbaaaaad);
    out3 = out3 | (out3 == 0xbaaaaad);
    wet_run = wet_run | (wet_run == 0xbaaaaad);
    trash = trash | (trash == 0xbaaaaad);
    
  
  
  
  
  
  trash = FORCE_READ(in - 128, trash);
  trash = FORCE_READ(out1 - 128, trash);
  trash = FORCE_READ(out2 - 128, trash);
  trash = FORCE_READ(out3 - 128, trash);
  
    

  mfence();
  lfence();
  
  const double start = DBL_MIN;
  const double denormal_result = DBL_MIN / 2;

  double divide_by = wet_run + 1;
  double result = start / divide_by;

  if (result == denormal_result)
    return trash;

  
  asm volatile("");
  if (!wet_run)
    return trash;

  
  
  

  
  trash = *(uintptr_t *)(in); 

  
  
  

  uintptr_t sum = 0;
  
  

  sum += *(uintptr_t *)(out1 + trash);
  sum += *(uintptr_t *)(out2 + trash);
  sum += *(uintptr_t *)(out3 + trash);

  return trash + sum;
}

uintptr_t fan_gate_impl_5(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, uintptr_t out4, uintptr_t out5,
                          bool wet_run, uintptr_t trash)
{
  
  
  
  
  
  
  
  
  
  
  
  
  

  
  
  
    in = in | (in == 0xbaaaaad);
    out1 = out1 | (out1 == 0xbaaaaad);
    out2 = out2 | (out2 == 0xbaaaaad);
    out3 = out3 | (out3 == 0xbaaaaad);
    out4 = out4 | (out4 == 0xbaaaaad);
    out5 = out5 | (out5 == 0xbaaaaad);
    wet_run = wet_run | (wet_run == 0xbaaaaad);
    trash = trash | (trash == 0xbaaaaad);
    
  
  
  
  
  
  
  
  trash = FORCE_READ(in - 128, trash);
  trash = FORCE_READ(out1 - 128, trash);
  trash = FORCE_READ(out2 - 128, trash);
  trash = FORCE_READ(out3 - 128, trash);
  trash = FORCE_READ(out4 - 128, trash);
  trash = FORCE_READ(out5 - 128, trash);
  

  mfence();
  lfence();
  
  const double start = DBL_MIN;
  const double denormal_result = DBL_MIN / 2;

  double divide_by = wet_run + 1;
  double result = start / divide_by;

  if (result == denormal_result)
    return trash;

  
  asm volatile("");
  if (!wet_run)
    return trash;

  
  
  

  
  trash = *(uintptr_t *)(in); 

  
  
  

  uintptr_t sum = 0;
  
  

  sum += *(uintptr_t *)(out1 + trash);
  sum += *(uintptr_t *)(out2 + trash);
  sum += *(uintptr_t *)(out3 + trash);
  sum += *(uintptr_t *)(out4 + trash);
  sum += *(uintptr_t *)(out5 + trash);

  return trash + sum;
}

uintptr_t fan_gate(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t trash)
{
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 2;
  #endif
  gate_metadata __attribute__((aligned(64))) fake = {0};
  
  
  

  trash = fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = fan_gate_impl(&fake, &fake, &fake, false, trash);
  trash = fan_gate_impl(in, out1, out2, true, trash);

  
  
  

  return trash;
}

uintptr_t fan_gate_3(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, 
                     uintptr_t trash)
{
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 3;
  #endif
  gate_metadata __attribute__((aligned(64))) fake = {0};
  
  
  

  trash = fan_gate_impl_3(&fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_3(&fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_3(&fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_3(&fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_3(in, out1, out2, out3, true, trash);

  
  
  

  return trash;
}

uintptr_t fan_gate_5(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, uintptr_t out4, uintptr_t out5,
                     uintptr_t trash)
{
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 5;
  #endif
  gate_metadata __attribute__((aligned(64))) fake = {0};
  
  
  

  trash = fan_gate_impl_5(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_5(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_5(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_5(&fake, &fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_5(in, out1, out2, out3, out4, out5, true, trash);

  
  
  

  return trash;
}

uintptr_t test_fan(uintptr_t trash)
{
  {
    
    printf("FAN GATE: \n");

    uintptr_t output1_read;
    uintptr_t output2_read;

    uintptr_t input = get_temporary_memory();
    uintptr_t output1 = get_temporary_memory();
    uintptr_t output2 = get_temporary_memory();

    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    clflush(output2);

    mfence();
    lfence();

    trash = fan_gate(input, output1, output2, trash);

    trash = read_addr(output1, &output1_read, trash);
    trash = read_addr(output2, &output2_read, trash);

    printf("0 : %d %d\n", output1_read, output2_read);

    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    clflush(output2);

    mfence();
    lfence();

    trash = FORCE_READ(input, trash);

    trash = fan_gate(input, output1, output2, trash);

    trash = read_addr(output1, &output1_read, trash);
    trash = read_addr(output2, &output2_read, trash);

    printf("0 : %d %d\n", output1_read, output2_read);

    return_temporary_memory(output2);
    return_temporary_memory(output1);
    return_temporary_memory(input);
  }
  return trash;
}


uintptr_t test_fan_3(uintptr_t trash)
{
  {
    
    printf("FAN GATE 3: \n");

    uintptr_t output1_read;
    uintptr_t output2_read;
    uintptr_t output3_read;

    uintptr_t input = get_temporary_memory();
    uintptr_t output1 = get_temporary_memory();
    uintptr_t output2 = get_temporary_memory();
    uintptr_t output3 = get_temporary_memory();

    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    clflush(output2);
    clflush(output3);

    mfence();
    lfence();

    trash = fan_gate_3(input, output1, output2, output3, trash);

    trash = read_addr(output1, &output1_read, trash);
    trash = read_addr(output2, &output2_read, trash);
    trash = read_addr(output3, &output3_read, trash);

    printf("0 : %d %d %d\n", output1_read, output2_read, output3_read);

    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    clflush(output2);
    clflush(output3);

    mfence();
    lfence();

    trash = FORCE_READ(input, trash);

    mfence();
    lfence();

    trash = fan_gate_3(input, output1, output2, output3, trash);

    trash = read_addr(output1, &output1_read, trash);
    trash = read_addr(output2, &output2_read, trash);
    trash = read_addr(output3, &output3_read, trash);

    printf("1 : %d %d %d\n", output1_read, output2_read, output3_read);

    return_temporary_memory(output3);
    return_temporary_memory(output2);
    return_temporary_memory(output1);
    return_temporary_memory(input);
  }
  return trash;
}

uintptr_t test_fan_accuracy(uintptr_t trash, uintptr_t iteration,
                            address_state state)
{

  uintptr_t correct = 0;
  uintptr_t incorrect = 0;

  uintptr_t output1_read;
  uintptr_t output2_read;

  uintptr_t input = get_temporary_memory();
  uintptr_t output1 = get_temporary_memory();
  uintptr_t output2 = get_temporary_memory();

  for (uintptr_t i = 0; i < iteration; i++)
  {
    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    clflush(output2);

    mfence();
    lfence();

    trash = fan_gate(input, output1, output2, trash);

    trash = read_addr(output1, &output1_read, trash);
    trash = read_addr(output2, &output2_read, trash);

    if (output1_read == 0)
      correct++;
    else
      incorrect++;
      
    if (output2_read == 0)
      correct++;
    else
      incorrect++;

    

    mfence();
    lfence();

    clflush(input);
    clflush(output1);
    clflush(output2);

    mfence();
    lfence();

    
    fetch_address(input, state);

    mfence();
    lfence();

    trash = fan_gate(input, output1, output2, trash);

    trash = read_addr(output1, &output1_read, trash);
    trash = read_addr(output2, &output2_read, trash);


    if (output1_read == 1)
      correct++;
    else
      incorrect++;
      
    if (output2_read == 1)
      correct++;
    else
      incorrect++;
  }
  
  printf("FAN Accuracy : %ld / %ld ", correct, correct + incorrect);
  print_state(state);

  return_temporary_memory(output2);
  return_temporary_memory(output1);
  return_temporary_memory(input);

  return trash;
}

uintptr_t fan_gate_impl_4(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, uintptr_t out4,
                          bool wet_run, uintptr_t trash)
{
  
  
  
  
  
  
    in = in | (in == 0xbaaaaad);
    out1 = out1 | (out1 == 0xbaaaaad);
    out2 = out2 | (out2 == 0xbaaaaad);
    out3 = out3 | (out3 == 0xbaaaaad);
    out4 = out4 | (out4 == 0xbaaaaad);
    wet_run = wet_run | (wet_run == 0xbaaaaad);
    trash = trash | (trash == 0xbaaaaad);
    
  
  
  
  
  
  
  trash = FORCE_READ(in - 128, trash);
  trash = FORCE_READ(out1 - 128, trash);
  trash = FORCE_READ(out2 - 128, trash);
  trash = FORCE_READ(out3 - 128, trash);
  trash = FORCE_READ(out4 - 128, trash);
  
    
    

  mfence();
  lfence();
  
  const double start = DBL_MIN;
  const double denormal_result = DBL_MIN / 2;

  double divide_by = wet_run + 1;
  double result = start / divide_by;

  if (result == denormal_result)
    return trash;

  
  asm volatile("");
  if (!wet_run)
    return trash;

  
  
  

  
  trash = *(uintptr_t *)(in); 

  
  
  

  uintptr_t sum = 0;
  
  

  sum += *(uintptr_t *)(out1 + trash);
  sum += *(uintptr_t *)(out2 + trash);
  sum += *(uintptr_t *)(out3 + trash);
  sum += *(uintptr_t *)(out4 + trash);

  return trash + sum;
}

uintptr_t fan_gate_4(uintptr_t in, uintptr_t out1, uintptr_t out2, uintptr_t out3, uintptr_t out4, uintptr_t trash)
{
  #ifdef GATES_STATS
    gates_count++;
    intermediate_values += 4;
  #endif
  gate_metadata __attribute__((aligned(64))) fake = {0};
  
  
  

  trash = fan_gate_impl_4(&fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_4(&fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_4(&fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_4(&fake, &fake, &fake, &fake, &fake, false, trash);
  trash = fan_gate_impl_4(in, out1, out2, out3, out4, true, trash);

  
  
  

  return trash;
}