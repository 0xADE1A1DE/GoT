#include <stdlib.h>
#include <time.h>

#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

#ifdef GATES_STATS
  #include <sys/time.h>
#endif

#define WORDSIZE 32

extern intptr_t prefetcher_enabled;

static const uint32_t K[] = {/* Constants defined in SHA-1   */
                             0x5A827999,
                             0x6ED9EBA1,
                             0x8F1BBCDC,
                             0xCA62C1D6};

/*
 *  Define the SHA1 circular left shift macro
 */
#define SHA1CircularShift(bits, word) \
  (((word) << (bits)) | ((word) >> (32 - (bits))))

static uintptr_t _32BitAnd(uintptr_t a[], uintptr_t b[], uintptr_t out[], uintptr_t trash)
{
#pragma GCC unroll 300
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    clflush(out[i]);
    mfence();
    lfence();
    trash = and_fan_gate(a[i], b[i], out[i], trash);
  }
  return trash;
}

static uintptr_t _32BitNot(uintptr_t in[], uintptr_t out[], uintptr_t trash)
{
#pragma GCC unroll 300
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    clflush(out[i]);
    mfence();
    lfence();
    trash = not_fan_gate(in[i], out[i], trash);
  }
  return trash;
}
static uintptr_t _32BitOr(uintptr_t a[], uintptr_t b[], uintptr_t out[], uintptr_t trash)
{
#pragma GCC unroll 300
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    clflush(out[i]);
    mfence();
    lfence();
    trash = or_from_nor_not(a[i], b[i], out[i], trash);
  }
  return trash;
}



































static uintptr_t LeftShift(uintptr_t **in, uintptr_t **out,
                           uintptr_t amount)
{
  uintptr_t temp[WORDSIZE];

#pragma GCC unroll 300
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    temp[i] = in[i];
  }

#pragma GCC unroll 300
  for (uintptr_t i = amount; i < WORDSIZE; i++)
  {
    out[i - amount] = temp[i];
  }

#pragma GCC unroll 300
  for (uintptr_t i = WORDSIZE - amount, j = 0; i < WORDSIZE; i++, j++)
  {
    out[i] = temp[j];
  }
}

static uintptr_t _32BitValueToCacheState(uint32_t value, uintptr_t addresses1[],
                                         uintptr_t addresses2[])
{
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    clflush(addresses1[i]);
    if (addresses2)
      clflush(addresses2[i]);
    mfence();
    lfence();

    if (value & (1u << i))
    {
      fetch_address(addresses1[i], L1);
      if (addresses2)
        fetch_address(addresses2[i], L1);
    }
  }
  mfence();
  lfence();
}











static uintptr_t _cacheStateTo32BitValue(uintptr_t addresses[], uintptr_t *output,
                                         uintptr_t trash)
{
  *output = 0;
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    uintptr_t read;
    trash = read_addr(addresses[i], &read, trash);
    mfence();
    lfence();
    *output = *output | ((uint32_t)read << i);
  }
  return trash;
}

extern uint64_t TARGET_SUCCESS_COUNTS;



extern uintptr_t temporary_memory[TEMP_MEMORY_SIZE];
extern uintptr_t available_temporary_memory;

uintptr_t sha1(uintptr_t trash, uintptr_t iteration)
{
  bool good_accuracy_found = false;
  uintptr_t temp_memory_backup[TEMP_MEMORY_SIZE];
  srand(time(NULL));
  uintptr_t target_accuracy_success = 0;
  
  uintptr_t first_iteration_array[TEMP_MEMORY_SIZE];
  
  uintptr_t fail_to_find = 0;
  
  while (true)
  {
    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    uintptr_t Intermediate_Hash[5];

    Intermediate_Hash[0] = 0x67452301;
    Intermediate_Hash[1] = 0xEFCDAB89;
    Intermediate_Hash[2] = 0x98BADCFE;
    Intermediate_Hash[3] = 0x10325476;
    Intermediate_Hash[4] = 0xC3D2E1F0;

    uint32_t A_arch = Intermediate_Hash[0];
    uint32_t B_arch = Intermediate_Hash[1];
    uint32_t C_arch = Intermediate_Hash[2];
    uint32_t D_arch = Intermediate_Hash[3];
    uint32_t E_arch = Intermediate_Hash[4];

    uint32_t A_arch_result;
    uint32_t B_arch_result;
    uint32_t C_arch_result;
    uint32_t D_arch_result;
    uint32_t E_arch_result;

    uint32_t temp_arch;
    uint32_t t;
    

    if (!good_accuracy_found) {
      randomise_temp_memory();
      
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++) {
        first_iteration_array[i] = temporary_memory[i];
      }
    }

    #ifdef GATES_STATS
      struct timeval start, stop;
      circuit_execution_time_us = 0.0;
      gates_count = 0;
      intermediate_values = 0;
    #endif

    for (uintptr_t i = 0; i < iteration; i++)
    {

      
      
      
      
      
      
      
      
      
      
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++) {
        temporary_memory[i] = first_iteration_array[i];
      }
      
      mfence();
      lfence();
      
    
    uint32_t W_arch = rand();
      
    
    for (t = 0; t < 1; t++)
    {
      temp_arch = SHA1CircularShift(5, A_arch) +
                  ((B_arch & C_arch) | ((~B_arch) & D_arch)) + E_arch + W_arch + K[0];
      E_arch_result = D_arch;
      D_arch_result = C_arch;
      C_arch_result = SHA1CircularShift(30, B_arch);
      B_arch_result = A_arch;
      A_arch_result = temp_arch;
    }

      
      uintptr_t A1[WORDSIZE];
      
      uintptr_t B1[WORDSIZE];
      uintptr_t B2[WORDSIZE];
      
      uintptr_t C1[WORDSIZE];
      
      uintptr_t D1[WORDSIZE];
      
      uintptr_t E[WORDSIZE];
      uintptr_t K0[WORDSIZE];
      uintptr_t W[WORDSIZE];

      uintptr_t B_AND_C[WORDSIZE];
      uintptr_t NOT_B[WORDSIZE];
      uintptr_t NOT_B_AND_D[WORDSIZE];
      uintptr_t B_AND_C_OR_NOT_B_AND_D[WORDSIZE];

      uintptr_t SUM_1[WORDSIZE];
      uintptr_t SUM_2[WORDSIZE];
      uintptr_t SUM_3[WORDSIZE];

      uintptr_t A_RESULT[WORDSIZE];

      for (uintptr_t i = 0; i < WORDSIZE; i++)
      {
        A1[i] = get_temporary_memory();
        
        B1[i] = get_temporary_memory();
        B2[i] = get_temporary_memory();
        
        C1[i] = get_temporary_memory();
        
        D1[i] = get_temporary_memory();
        
        E[i] = get_temporary_memory();
        K0[i] = get_temporary_memory();
        W[i] = get_temporary_memory();

        B_AND_C[i] = get_temporary_memory();

        NOT_B[i] = get_temporary_memory();
        NOT_B_AND_D[i] = get_temporary_memory();

        B_AND_C_OR_NOT_B_AND_D[i] = get_temporary_memory();

        SUM_1[i] = get_temporary_memory();
        SUM_2[i] = get_temporary_memory();
        SUM_3[i] = get_temporary_memory();

        A_RESULT[i] = get_temporary_memory();
      }

      _32BitValueToCacheState(A_arch, A1, NULL);

      for (uintptr_t i = 0; i < WORDSIZE; i++)
      {
        clflush(B1[i]);
        clflush(B2[i]);
        
        mfence();
        lfence();

        if (B_arch & ((uint32_t)1 << i))
        {
          fetch_address(B1[i], L1);
          fetch_address(B2[i], L1);
          
        }
      }

      
      _32BitValueToCacheState(C_arch, C1, NULL);
      _32BitValueToCacheState(D_arch, D1, NULL);
      _32BitValueToCacheState(E_arch, E, NULL);
      _32BitValueToCacheState(K[0], K0, NULL);
      _32BitValueToCacheState(W_arch, W, NULL);


      
      #ifdef GATES_STATS
        gettimeofday(&start, NULL);
      #endif


      LeftShift(A1, A1, 27);
      
      mfence();
      lfence();



      trash = _32BitAnd(B1, C1, B_AND_C, trash);

      mfence();
      lfence();

      trash = _32BitNot(B2, NOT_B, trash);

      mfence();
      lfence();

      trash = _32BitAnd(NOT_B, D1, NOT_B_AND_D, trash);

      mfence();
      lfence();

      trash = _32BitOr(B_AND_C, NOT_B_AND_D, B_AND_C_OR_NOT_B_AND_D, trash);

      mfence();
      lfence();

      trash = _32BitAdder_impl(A1, B_AND_C_OR_NOT_B_AND_D, SUM_1, WORDSIZE, trash);

      mfence();
      lfence();

      trash = _32BitAdder_impl(SUM_1, E, SUM_2, WORDSIZE, trash);

      mfence();
      lfence();

      trash = _32BitAdder_impl(SUM_2, W, SUM_3, WORDSIZE, trash);

      mfence();
      lfence();

      trash = _32BitAdder_impl(SUM_3, K0, A_RESULT, WORDSIZE, trash);

      mfence();
      lfence();

      
      





      
      
      
      #ifdef GATES_STATS
        gettimeofday(&stop, NULL);
        circuit_execution_time_us += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
      #endif
      






      
      

      uintptr_t A_READ = 0;
      uintptr_t B_READ = A_arch;
      uintptr_t C_READ = SHA1CircularShift(30, B_arch);
      uintptr_t D_READ = C_arch;
      uintptr_t E_READ = D_arch;

      trash = _cacheStateTo32BitValue(A_RESULT, &A_READ, trash);
      
      
      
      

      
      
      
      
      
      
      
      
      
      
      
      
      


      if (A_READ == A_arch_result &&
          B_READ == B_arch_result &&
          C_READ == C_arch_result &&
          D_READ == D_arch_result) {
        correct++;
      } else {
        incorrect++;
      }
      for (intptr_t i = WORDSIZE - 1; i >= 0; i--)
      {
        return_temporary_memory(A_RESULT[i]);
        return_temporary_memory(SUM_3[i]);
        return_temporary_memory(SUM_2[i]);
        return_temporary_memory(SUM_1[i]);
        return_temporary_memory(B_AND_C_OR_NOT_B_AND_D[i]);
        return_temporary_memory(NOT_B_AND_D[i]);
        return_temporary_memory(NOT_B[i]);
        return_temporary_memory(B_AND_C[i]);
        return_temporary_memory(W[i]);
        return_temporary_memory(K0[i]);
        return_temporary_memory(E[i]);
        
        return_temporary_memory(D1[i]);
        
        return_temporary_memory(C1[i]);
        
        return_temporary_memory(B2[i]);
        return_temporary_memory(B1[i]);
        
        return_temporary_memory(A1[i]);
      }
      
      
      
      
      
      
    }
    
    #ifdef GATES_STATS
      printf("Execution time averaged over %ld runs: %lf\n", iteration, circuit_execution_time_us / (double)iteration);
      printf("Gates count: %lf\n", gates_count / (double)iteration);
      printf("Intermediate values: %lf\n", intermediate_values / (double)iteration);
    #endif

  double TARGET_ACCURACY = 1.0;
  
  if (prefetcher_enabled)
    TARGET_ACCURACY = 0.5;
    



    const double accuracy = (double)correct / (double)(incorrect + correct);
    
    
    if (good_accuracy_found || accuracy >= TARGET_ACCURACY) {
    printf("SHA1 Accuracy : %d / %d (%lf) \n", correct, correct + incorrect,
           (double)correct / (double)(correct + incorrect));
    }

#define FAIL_TO_FIND_THRESHOLD 1000
    if (accuracy < TARGET_ACCURACY && !good_accuracy_found)
    {
      printf("accuracy fell short, randomising temp memory!\n");
      target_accuracy_success = 0;
      fail_to_find++;
      
      if (fail_to_find >= FAIL_TO_FIND_THRESHOLD) {
        return trash;
      }
    }
    else
    {
      if (!good_accuracy_found) {
        good_accuracy_found = true;
        
        
        
      } else {
        
        
        
      }
      target_accuracy_success++;
      if (target_accuracy_success >= TARGET_SUCCESS_COUNTS)
      {
        printf("target accuracy reached! continuing test\n");
        break;
      }
      else
      {
        printf("Target accuracy reached, doing %ld more time(s) with the same configuration\n",
               TARGET_SUCCESS_COUNTS - target_accuracy_success);
      }
    }
  }

  return trash;
}
