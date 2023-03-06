#include <stdlib.h>
#include <time.h>

#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

#include <stdio.h>
#include <string.h>

#define WORDSIZE 32

extern void *clear_memory;
extern int pht_nuke();

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

static uintptr_t _32BitXor(uintptr_t a[], uintptr_t b[], uintptr_t out[], uintptr_t trash)
{

  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    clflush(out[i]);
    mfence();
    lfence();
    trash = xor_impl(a[i], b[i], out[i], trash);
  }
  return trash;
}

static uintptr_t _32BitAnd(uintptr_t a[], uintptr_t b[], uintptr_t out[], uintptr_t trash)
{

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
                                         uintptr_t addresses2[], uintptr_t addresses3[])
{
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    mfence();
    lfence();
    clflush(addresses1[i]);
    if (addresses2) {
    mfence();
    lfence();
      clflush(addresses2[i]);
    }
    if (addresses3) {
    mfence();
    lfence();
      clflush(addresses3[i]);
    }
    mfence();
    lfence();

    if (value & (1u << i))
    {
      fetch_address(addresses1[i], L1);
      if (addresses2)
        fetch_address(addresses2[i], L1);
      if (addresses3)
        fetch_address(addresses3[i], L1);
    }
  mfence();
  lfence();
  }
}











static uintptr_t _cacheStateTo32BitValue(uintptr_t addresses[], uint32_t *output,
                                         uintptr_t trash)
{
  *output = 0;
  for (uintptr_t i = 0; i < WORDSIZE; i++)
  {
    mfence();
    lfence();
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

static inline copy_state(uintptr_t *dest, uintptr_t *src)
{
  for (int i = 0; i < WORDSIZE; i++)
  {
    dest[i] = src[i];
  }
}

#define NUMBER_VOTING 10

static inline void get_majority_all(uint32_t *A, uint32_t *B, uint32_t *C, uint32_t *D, uint32_t *E,
                                    uint32_t *A_result, uint32_t *B_result, uint32_t *C_result, uint32_t *D_result, uint32_t *E_result, int number_voting)
{ 
  uint64_t max_same = 0;
  for (int i = 0; i < number_voting; i++)
  {
    uint64_t current_same = 0;
    for (int k = i; k < number_voting; k++)
    {
      if (A[i] == A[k] &&
          B[i] == B[k] &&
          C[i] == C[k] &&
          D[i] == D[k] &&
          E[i] == E[k])
      {
        current_same++;
      }
    }
    if (current_same > max_same)
    {
      max_same = current_same;
      *A_result = A[i];
      *B_result = B[i];
      *C_result = C[i];
      *D_result = D[i];
      *E_result = E[i];
    }
  }
  
  return;
}

static inline void print_binary(uint32_t hex) {
  printf("\t\t");
  for (int i = 31; i >= 0; i--) {
    if (hex & (1 << i)) 
      printf("1");
    else
      printf("0");
  }
  printf("\n");
}



uint64_t DECOY_TARGET_SUCCESS_COUNTS = 10;

uintptr_t sha1_decoy(uintptr_t trash, uintptr_t iteration)
{
  
  
  bool good_accuracy_found = false;
  uintptr_t temp_memory_backup[TEMP_MEMORY_SIZE];
  
  uintptr_t target_accuracy_success = 0;
  
  uintptr_t first_iteration_array[TEMP_MEMORY_SIZE];
  
  uintptr_t fail_to_find = 0;
  

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
      
      
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++) {
        first_iteration_array[i] = temporary_memory[i];
      }
    }


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
      uintptr_t A2[WORDSIZE];
      uintptr_t B1[WORDSIZE];
      uintptr_t B2[WORDSIZE];
      uintptr_t B3[WORDSIZE];
      uintptr_t C1[WORDSIZE];
      uintptr_t C2[WORDSIZE];
      uintptr_t D1[WORDSIZE];
      uintptr_t D2[WORDSIZE];
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
        A2[i] = get_temporary_memory();
        B1[i] = get_temporary_memory();
        B2[i] = get_temporary_memory();
        B3[i] = get_temporary_memory();
        C1[i] = get_temporary_memory();
        C2[i] = get_temporary_memory();
        D1[i] = get_temporary_memory();
        D2[i] = get_temporary_memory();
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

      _32BitValueToCacheState(A_arch, A1, A2, NULL);
      _32BitValueToCacheState(B_arch, B1, B2, B3);
      _32BitValueToCacheState(C_arch, C1, C2, NULL);
      _32BitValueToCacheState(D_arch, D1, D2, NULL);
      _32BitValueToCacheState(E_arch, E, NULL, NULL);
      _32BitValueToCacheState(K[0], K0, NULL, NULL);
      _32BitValueToCacheState(W_arch, W, NULL, NULL);




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

      uintptr_t E_RESULT = D2;
      uintptr_t D_RESULT = C2;





      
      LeftShift(B3, B3, 2);
      
    
      






      
      
      
      
      
      
      
      
      
      
      
      
      


  
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
        return_temporary_memory(D2[i]);
        return_temporary_memory(D1[i]);
        return_temporary_memory(C2[i]);
        return_temporary_memory(C1[i]);
        return_temporary_memory(B3[i]);
        return_temporary_memory(B2[i]);
        return_temporary_memory(B1[i]);
        return_temporary_memory(A2[i]);
        return_temporary_memory(A1[i]);
      }
      
      
      
      
      
      
    }
  
  

  return trash;
}










uintptr_t sha1(uintptr_t trash, uintptr_t iteration_p)
{
  
  
  
  
      
  
  
  
  
  
  
  
      
      
      
  
  
  
  
  
        
  
  
  
  
  
  
  
  
  
  
  
  
  bool good_accuracy_found = false;
  srand(time(NULL));
  uintptr_t target_accuracy_success = 0;

  uintptr_t first_iteration_array_set_0[TEMP_MEMORY_SIZE];
  uintptr_t first_iteration_array_set_1[TEMP_MEMORY_SIZE];
  uintptr_t first_iteration_array_set_2[TEMP_MEMORY_SIZE];
  uintptr_t first_iteration_array_set_3[TEMP_MEMORY_SIZE];
  
  uintptr_t set_training = 0;

  uintptr_t fail_to_find = 0;
  
  uintptr_t Intermediate_Hash[5];
  uintptr_t Initial_Intermediate_Hash[5];

  Initial_Intermediate_Hash[0] = 0x67452301;
  Initial_Intermediate_Hash[1] = 0xEFCDAB89;
  Initial_Intermediate_Hash[2] = 0x98BADCFE;
  Initial_Intermediate_Hash[3] = 0x10325476;
  Initial_Intermediate_Hash[4] = 0xC3D2E1F0;





  while (true)
  {
    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    if (!good_accuracy_found)
    {
      randomise_temp_memory();
      printf("RANDOMISED!!\n");
      
      
      
      
      
      
      
      
      
      
      
      
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
      {

          first_iteration_array_set_0[i] = temporary_memory[i];
      }
    }
    
#define NUMBER_BLOCKS 2
    uintptr_t iteration;
    
    if (!good_accuracy_found)
      iteration = 1;
    else
      iteration = iteration_p;
      
    int round_retry = 0;
    uint32_t W_arch;
    
    double total_retries_required;

    for (uintptr_t itr = 0; itr < iteration; itr++)
    {
      total_retries_required = 0;
      
      
      
      
      
      
      
      
        uintptr_t number_blocks = NUMBER_BLOCKS;
        if (!good_accuracy_found) {
          number_blocks = 1;
        }

      

        for (uintptr_t block = 0; block < number_blocks; block++)
        {

          if (block == 0)
          {
            Intermediate_Hash[0] = Initial_Intermediate_Hash[0];
            Intermediate_Hash[1] = Initial_Intermediate_Hash[1];
            Intermediate_Hash[2] = Initial_Intermediate_Hash[2];
            Intermediate_Hash[3] = Initial_Intermediate_Hash[3];
            Intermediate_Hash[4] = Initial_Intermediate_Hash[4];
          }

      
#define TEST_ROUND 80
      
      uint32_t A_arch;
      uint32_t B_arch;
      uint32_t C_arch;
      uint32_t D_arch;
      uint32_t E_arch;
      
      
      
      
        
        A_arch = Intermediate_Hash[0];
        B_arch = Intermediate_Hash[1];
        C_arch = Intermediate_Hash[2];
        D_arch = Intermediate_Hash[3];
        E_arch = Intermediate_Hash[4];
        
        
        
        
        
        
        
        
        
        
        
        
        
      
      

      uint32_t A_arch_result;
      uint32_t B_arch_result;
      uint32_t C_arch_result;
      uint32_t D_arch_result;
      uint32_t E_arch_result;

      uint32_t temp_arch;

      






































      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      

      
      
      
      

      mfence();
      lfence();



      
      
      
      
      

      
      
      
      
      
      
      
      

      uint32_t A_start;
      uint32_t B_start;
      uint32_t C_start;
      uint32_t D_start;
      uint32_t E_start;

      uint32_t A_read[NUMBER_VOTING];
      uint32_t B_read[NUMBER_VOTING];
      uint32_t C_read[NUMBER_VOTING];
      uint32_t D_read[NUMBER_VOTING];
      uint32_t E_read[NUMBER_VOTING];

      
      int k;
      int number_rounds;
      
      
      
      
        k = 0;
      
      
      
      
      
        number_rounds = TEST_ROUND;
        
        for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
        {
              temporary_memory[i] = first_iteration_array_set_0[i];
        }
        
        
      #ifdef GATES_STATS
      double total_gates_count = 0;
      double total_intermediate_values = 0;
      double total_values_read = 0;
      #endif

      for (; k < number_rounds; k++)
      {
        
        
        
        
        
        
        
        
        
            
        
        
        
        
        
        
        
        
        

        if (!round_retry) {
        W_arch = rand();
        if (k <= 19)
        {
          temp_arch = SHA1CircularShift(5, A_arch) +
                      ((B_arch & C_arch) | ((~B_arch) & D_arch)) + E_arch + W_arch + K[0];
        }
        else if (k <= 39)
        {
          temp_arch = SHA1CircularShift(5, A_arch) +
                      (B_arch ^ C_arch ^ D_arch) + E_arch + W_arch + K[1];
        }
        else if (k <= 59)
        {
          temp_arch = SHA1CircularShift(5, A_arch) +
                      ((B_arch & C_arch) | (B_arch & D_arch) | (C_arch & D_arch)) + E_arch + W_arch + K[2];
        }
        else if (k <= 79)
        {
          temp_arch = SHA1CircularShift(5, A_arch) +
                      (B_arch ^ C_arch ^ D_arch) + E_arch + W_arch + K[3];
        }
        E_arch = D_arch;
        D_arch = C_arch;
        C_arch = SHA1CircularShift(30, B_arch);
        B_arch = A_arch;
        A_arch = temp_arch;
        }
        round_retry = 0;
      

      E_arch_result = E_arch;
      D_arch_result = D_arch;
      C_arch_result = C_arch;
      B_arch_result = B_arch;
      A_arch_result = A_arch;
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
        {
          if (good_accuracy_found) {
              temporary_memory[i] = first_iteration_array_set_0[i];
          }
        }
        

        int voting_number;
        
        
        
          voting_number = NUMBER_VOTING;
          
        
        
        
        
          
        for (int voting_round = 0; voting_round < voting_number; voting_round++)
        {
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
          

          
          
          
          
          mfence();
          lfence();
          
          for (int nuke = 0; nuke < 1; nuke++)
            pht_nuke();
          
          
          
          
          
          
          
          
            mfence();
            lfence();
            trash = sha1_decoy(trash, 25);
          

          #ifdef GATES_STATS
            gates_count = 0;
            intermediate_values = 0;
          #endif

          mfence();
          lfence();
          
          if (k == 0)
          
          {
            
            A_start = Intermediate_Hash[0];
            B_start = Intermediate_Hash[1];
            C_start = Intermediate_Hash[2];
            D_start = Intermediate_Hash[3];
            E_start = Intermediate_Hash[4];
            
            
            
            
            
            
            

            
            
            
            
            
            
          }

          
          
          
          
          

          mfence();
          lfence();

          
          
          
          
          
          
          
          

          if (k <= 19)
          {
            mfence();
            lfence();
            

            
            
            
            
            
            
            

            
            
            
            
            

            
            
            

            

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

            _32BitValueToCacheState(A_start, A1, NULL, NULL);

            
            
            
            
            
            
            

            
            
            
            
            
            
            
            
            

            _32BitValueToCacheState(B_start, B1, B2, NULL);
            _32BitValueToCacheState(C_start, C1, NULL, NULL);
            _32BitValueToCacheState(D_start, D1, NULL, NULL);
            _32BitValueToCacheState(E_start, E, NULL, NULL);
            _32BitValueToCacheState(K[0], K0, NULL, NULL);
            _32BitValueToCacheState(W_arch, W, NULL, NULL);

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
            
            LeftShift(A1, A1, 27);
            
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

            
            
            
            
            

            trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
            
            mfence();
            lfence();
          
            
            E_read[voting_round] = D_start;
            D_read[voting_round] = C_start;
            C_read[voting_round] = SHA1CircularShift(30, B_start);
            B_read[voting_round] = A_start;


            
            
            
            
            

            

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

          else if (k <= 39)
          {
            mfence();
            lfence();
            uintptr_t A1[WORDSIZE];
            

            uintptr_t B1[WORDSIZE];
            

            uintptr_t C1[WORDSIZE];
            

            uintptr_t D1[WORDSIZE];
            

            uintptr_t E[WORDSIZE];

            uintptr_t K1[WORDSIZE];
            uintptr_t W[WORDSIZE];

            uintptr_t B_XOR_C[WORDSIZE];
            uintptr_t B_XOR_C_XOR_D[WORDSIZE];

            uintptr_t SUM_1[WORDSIZE];
            uintptr_t SUM_2[WORDSIZE];
            uintptr_t SUM_3[WORDSIZE];

            uintptr_t A_RESULT[WORDSIZE];

            for (uintptr_t i = 0; i < WORDSIZE; i++)
            {
              A1[i] = get_temporary_memory();
              
              B1[i] = get_temporary_memory();
              
              C1[i] = get_temporary_memory();
              
              D1[i] = get_temporary_memory();
              
              E[i] = get_temporary_memory();
              K1[i] = get_temporary_memory();
              W[i] = get_temporary_memory();

              B_XOR_C[i] = get_temporary_memory();
              B_XOR_C_XOR_D[i] = get_temporary_memory();

              SUM_1[i] = get_temporary_memory();
              SUM_2[i] = get_temporary_memory();
              SUM_3[i] = get_temporary_memory();

              A_RESULT[i] = get_temporary_memory();
            }
            _32BitValueToCacheState(A_start, A1, NULL, NULL);
            _32BitValueToCacheState(B_start, B1, NULL, NULL);
            _32BitValueToCacheState(C_start, C1, NULL, NULL);
            _32BitValueToCacheState(D_start, D1, NULL, NULL);
            _32BitValueToCacheState(E_start, E, NULL, NULL);
            
            _32BitValueToCacheState(K[1], K1, NULL, NULL);
            _32BitValueToCacheState(W_arch, W, NULL, NULL);

            

            mfence();
            lfence();

            trash = _32BitXor(B1, C1, B_XOR_C, trash);

            mfence();
            lfence();

            trash = _32BitXor(B_XOR_C, D1, B_XOR_C_XOR_D, trash);

            mfence();
            lfence();
            
            LeftShift(A1, A1, 27);
            
            mfence();
            lfence();

            trash = _32BitAdder_impl(A1, B_XOR_C_XOR_D, SUM_1, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_1, E, SUM_2, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_2, W, SUM_3, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_3, K1, A_RESULT, WORDSIZE, trash);

            mfence();
            lfence();

            
            
            
            
            
          
            mfence();
            lfence();
            
            trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
            
            mfence();
            lfence();
            
            E_read[voting_round] = D_start;
            D_read[voting_round] = C_start;
            C_read[voting_round] = SHA1CircularShift(30, B_start);
            B_read[voting_round] = A_start;
            
            
            
            
            

            for (intptr_t i = WORDSIZE - 1; i >= 0; i--)
            {
              return_temporary_memory(A_RESULT[i]);
              return_temporary_memory(SUM_3[i]);
              return_temporary_memory(SUM_2[i]);
              return_temporary_memory(SUM_1[i]);
              return_temporary_memory(B_XOR_C_XOR_D[i]);
              return_temporary_memory(B_XOR_C[i]);
              return_temporary_memory(W[i]);
              return_temporary_memory(K1[i]);
              return_temporary_memory(E[i]);
              
              return_temporary_memory(D1[i]);
              
              return_temporary_memory(C1[i]);
              
              return_temporary_memory(B1[i]);
              
              return_temporary_memory(A1[i]);
            }
          }

          else if (k <= 59)
          {
            
            mfence();
            lfence();
            uintptr_t A1[WORDSIZE];
            
            uintptr_t B1[WORDSIZE];
            uintptr_t B2[WORDSIZE];
            
            uintptr_t C1[WORDSIZE];
            uintptr_t C2[WORDSIZE];
            
            uintptr_t D1[WORDSIZE];
            uintptr_t D2[WORDSIZE];
            
            uintptr_t E[WORDSIZE];
            uintptr_t K2[WORDSIZE];
            uintptr_t W[WORDSIZE];

            uintptr_t B_AND_C[WORDSIZE];
            uintptr_t B_AND_D[WORDSIZE];
            uintptr_t C_AND_D[WORDSIZE];

            uintptr_t B_AND_C_OR_B_AND_D[WORDSIZE];
            uintptr_t B_AND_C_OR_B_AND_D_OR_C_AND_D[WORDSIZE];

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
              C2[i] = get_temporary_memory();
              

              D1[i] = get_temporary_memory();
              D2[i] = get_temporary_memory();
              
              E[i] = get_temporary_memory();
              K2[i] = get_temporary_memory();
              W[i] = get_temporary_memory();

              B_AND_C[i] = get_temporary_memory();
              B_AND_D[i] = get_temporary_memory();
              C_AND_D[i] = get_temporary_memory();

              B_AND_C_OR_B_AND_D[i] = get_temporary_memory();
              B_AND_C_OR_B_AND_D_OR_C_AND_D[i] = get_temporary_memory();

              SUM_1[i] = get_temporary_memory();
              SUM_2[i] = get_temporary_memory();
              SUM_3[i] = get_temporary_memory();

              A_RESULT[i] = get_temporary_memory();
            }

            

            _32BitValueToCacheState(A_start, A1, NULL, NULL);
            _32BitValueToCacheState(B_start, B1, B2, NULL);
            _32BitValueToCacheState(C_start, C1, C2, NULL);
            _32BitValueToCacheState(D_start, D1, D2, NULL);
            _32BitValueToCacheState(E_start, E, NULL, NULL);
            
            _32BitValueToCacheState(K[2], K2, NULL, NULL);
            _32BitValueToCacheState(W_arch, W, NULL, NULL);

            mfence();
            lfence();

            trash = _32BitAnd(B1, C1, B_AND_C, trash);

            mfence();
            lfence();

            trash = _32BitAnd(B2, D1, B_AND_D, trash);

            mfence();
            lfence();

            trash = _32BitAnd(C2, D2, C_AND_D, trash);

            mfence();
            lfence();

            trash = _32BitOr(B_AND_C, B_AND_D, B_AND_C_OR_B_AND_D, trash);

            mfence();
            lfence();

            trash = _32BitOr(B_AND_C_OR_B_AND_D, C_AND_D, B_AND_C_OR_B_AND_D_OR_C_AND_D, trash);

            mfence();
            lfence();
            
            LeftShift(A1, A1, 27);
            
            mfence();
            lfence();

            trash = _32BitAdder_impl(A1, B_AND_C_OR_B_AND_D_OR_C_AND_D, SUM_1, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_1, E, SUM_2, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_2, W, SUM_3, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_3, K2, A_RESULT, WORDSIZE, trash);

            mfence();
            lfence();

            
            
            
            
            
            

            
            
            
            
            
          

            trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
            
            mfence();
            lfence();
            
            E_read[voting_round] = D_start;
            D_read[voting_round] = C_start;
            C_read[voting_round] = SHA1CircularShift(30, B_start);
            B_read[voting_round] = A_start;

            
            
            
            


            for (intptr_t i = WORDSIZE - 1; i >= 0; i--)
            {
              return_temporary_memory(A_RESULT[i]);
              return_temporary_memory(SUM_3[i]);
              return_temporary_memory(SUM_2[i]);
              return_temporary_memory(SUM_1[i]);
              return_temporary_memory(B_AND_C_OR_B_AND_D_OR_C_AND_D[i]);
              return_temporary_memory(B_AND_C_OR_B_AND_D[i]);
              return_temporary_memory(C_AND_D[i]);
              return_temporary_memory(B_AND_D[i]);
              return_temporary_memory(B_AND_C[i]);
              return_temporary_memory(W[i]);
              return_temporary_memory(K2[i]);
              return_temporary_memory(E[i]);
              
              return_temporary_memory(D2[i]);
              return_temporary_memory(D1[i]);
              
              return_temporary_memory(C2[i]);
              return_temporary_memory(C1[i]);
              
              return_temporary_memory(B2[i]);
              return_temporary_memory(B1[i]);
              
              return_temporary_memory(A1[i]);
            }

            
            
            
            
            
          }
          
          else if (k <= 79)
          {
            mfence();
            lfence();
            uintptr_t A1[WORDSIZE];
            

            uintptr_t B1[WORDSIZE];
            

            uintptr_t C1[WORDSIZE];
            

            uintptr_t D1[WORDSIZE];
            

            uintptr_t E[WORDSIZE];

            uintptr_t K3[WORDSIZE];
            uintptr_t W[WORDSIZE];

            uintptr_t B_XOR_C[WORDSIZE];
            uintptr_t B_XOR_C_XOR_D[WORDSIZE];

            uintptr_t SUM_1[WORDSIZE];
            uintptr_t SUM_2[WORDSIZE];
            uintptr_t SUM_3[WORDSIZE];

            uintptr_t A_RESULT[WORDSIZE];

            for (uintptr_t i = 0; i < WORDSIZE; i++)
            {
              A1[i] = get_temporary_memory();
              
              B1[i] = get_temporary_memory();
              
              C1[i] = get_temporary_memory();
              
              D1[i] = get_temporary_memory();
              
              E[i] = get_temporary_memory();
              K3[i] = get_temporary_memory();
              W[i] = get_temporary_memory();

              B_XOR_C[i] = get_temporary_memory();
              B_XOR_C_XOR_D[i] = get_temporary_memory();

              SUM_1[i] = get_temporary_memory();
              SUM_2[i] = get_temporary_memory();
              SUM_3[i] = get_temporary_memory();

              A_RESULT[i] = get_temporary_memory();
            }
            _32BitValueToCacheState(A_start, A1, NULL, NULL);
            _32BitValueToCacheState(B_start, B1, NULL, NULL);
            _32BitValueToCacheState(C_start, C1, NULL, NULL);
            _32BitValueToCacheState(D_start, D1, NULL, NULL);
            _32BitValueToCacheState(E_start, E, NULL, NULL);
            
            _32BitValueToCacheState(K[3], K3, NULL, NULL);
            _32BitValueToCacheState(W_arch, W, NULL, NULL);

            

            mfence();
            lfence();

            trash = _32BitXor(B1, C1, B_XOR_C, trash);

            mfence();
            lfence();

            trash = _32BitXor(B_XOR_C, D1, B_XOR_C_XOR_D, trash);

            mfence();
            lfence();
            
            LeftShift(A1, A1, 27);
            
            mfence();
            lfence();

            trash = _32BitAdder_impl(A1, B_XOR_C_XOR_D, SUM_1, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_1, E, SUM_2, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_2, W, SUM_3, WORDSIZE, trash);

            mfence();
            lfence();

            trash = _32BitAdder_impl(SUM_3, K3, A_RESULT, WORDSIZE, trash);

            mfence();
            lfence();

            
            
            
            
            

            trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
            
            mfence();
            lfence();
            
            E_read[voting_round] = D_start;
            D_read[voting_round] = C_start;
            C_read[voting_round] = SHA1CircularShift(30, B_start);
            B_read[voting_round] = A_start;
            
            
            
            
            

            for (intptr_t i = WORDSIZE - 1; i >= 0; i--)
            {
              return_temporary_memory(A_RESULT[i]);
              return_temporary_memory(SUM_3[i]);
              return_temporary_memory(SUM_2[i]);
              return_temporary_memory(SUM_1[i]);
              return_temporary_memory(B_XOR_C_XOR_D[i]);
              return_temporary_memory(B_XOR_C[i]);
              return_temporary_memory(W[i]);
              return_temporary_memory(K3[i]);
              return_temporary_memory(E[i]);
              
              return_temporary_memory(D1[i]);
              
              return_temporary_memory(C1[i]);
              
              return_temporary_memory(B1[i]);
              
              return_temporary_memory(A1[i]);
            }
          }
          
          
          
          #ifdef GATES_STATS
          total_gates_count += gates_count;
          total_intermediate_values += intermediate_values;
          #endif
          
          #ifdef GATES_STATS
            total_values_read += 32;
          #endif
        
          
        }
        
        uintptr_t A_start_new = 0;
        uintptr_t B_start_new = 0;
        uintptr_t C_start_new = 0;
        uintptr_t D_start_new = 0;
        uintptr_t E_start_new = 0;
        

        get_majority_all(A_read, B_read, C_read, D_read, E_read,
                         &A_start_new, &B_start_new, &C_start_new, &D_start_new, &E_start_new, voting_number);
        
        
      
	
	
        
        
	
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        if (!(A_start_new == A_arch_result &&
            B_start_new == B_arch_result &&
            C_start_new == C_arch_result &&
            D_start_new == D_arch_result &&
            E_start_new == E_arch_result) && !good_accuracy_found)
        {
          
          total_retries_required += voting_number;
          
          
          
          
          printf("WRONG! BREAKING AT ROUND %d!!\n", k);
          
            
            break;
          
          round_retry = 1;
          k--;
          
        }
        else {
          A_start = A_start_new;
          B_start = B_start_new;
          C_start = C_start_new;
          D_start = D_start_new;
          E_start = E_start_new;
        }
      
      
      
      
      
      
      
          
          
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
          
      

      }
      

      
      
      
      
      
      

      
      
      
      
      


      #ifdef GATES_STATS
        printf("Gates count: %lf, intermediate values: %lf, architectural read: %lf, ratio: %lf\n",
               total_gates_count, total_intermediate_values, total_values_read, total_values_read / total_intermediate_values);
      #endif
      
      if ((int)total_retries_required <= 0) {
        correct++;
      } else {
        incorrect++;
      }
       printf("Total runs = %lf\n", (total_retries_required / (double)TEST_ROUND) + (double)NUMBER_VOTING);
    
      
      
      
      
        

          uintptr_t Expected_Intermediate_Hash[5] = {
            Intermediate_Hash[0] + A_arch_result,
            Intermediate_Hash[1] + B_arch_result,
            Intermediate_Hash[2] + C_arch_result,
            Intermediate_Hash[3] + D_arch_result,
            Intermediate_Hash[4] + E_arch_result
          };
        

        Intermediate_Hash[0] += (uintptr_t)A_start;
        Intermediate_Hash[1] += (uintptr_t)B_start;
        Intermediate_Hash[2] += (uintptr_t)C_start;
        Intermediate_Hash[3] += (uintptr_t)D_start;
        Intermediate_Hash[4] += (uintptr_t)E_start;

        printf("##### BLOCK %d #####\n", block);
        
        
        
        
        
        

        if ((uint32_t)Expected_Intermediate_Hash[0] == (uint32_t)Intermediate_Hash[0] &&
            (uint32_t)Expected_Intermediate_Hash[1] == (uint32_t)Intermediate_Hash[1] &&
            (uint32_t)Expected_Intermediate_Hash[2] == (uint32_t)Intermediate_Hash[2] &&
            (uint32_t)Expected_Intermediate_Hash[3] == (uint32_t)Intermediate_Hash[3] &&
            (uint32_t)Expected_Intermediate_Hash[4] == (uint32_t)Intermediate_Hash[4]) {
          printf("Block correct!  -- retries = %lf\n", total_retries_required);
          if (block == NUMBER_BLOCKS - 1) {
          if (total_retries_required == 0) {
            printf("Both block correct with no retries!\n");
          } else {
            printf("Both block correct with retries!\n");
          }
          }
        } else {
          printf("Block incorrect!\n");
          
        }




        
      
        }
        nextItr:
        
    fflush(stdout);
    mfence();
    lfence();
       
    }
#define TARGET_ACCURACY 0.98 
                            

    
    
    
    
    
    
    
    

#define FAIL_TO_FIND_THRESHOLD 1000
    if (total_retries_required > 0)
    {
      printf("accuracy fell short, randomising temp memory!\n");
      target_accuracy_success = 0;
      fail_to_find++;
      
      if (fail_to_find >= FAIL_TO_FIND_THRESHOLD)
      {
        return trash;
      }
    }
    else
    {
      if (!good_accuracy_found)
      {
        
        
          good_accuracy_found = true;
        
        
        
        
      }
      else
      {
        
        
        
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
