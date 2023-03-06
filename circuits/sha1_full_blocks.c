#include <stdlib.h>
#include <time.h>

#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

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
    if (addresses2)
    {
      mfence();
      lfence();
      clflush(addresses2[i]);
    }
    if (addresses3)
    {
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

static inline void print_binary(uint32_t hex)
{
  printf("\t\t");
  for (int i = 31; i >= 0; i--)
  {
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

  if (!good_accuracy_found)
  {
    
    
    for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
    {
      first_iteration_array[i] = temporary_memory[i];
    }
  }

  for (uintptr_t i = 0; i < iteration; i++)
  {

    
    
    
    
    
    
    
    

    for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
    {
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

#define SHA1HashSize 20

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
      
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
      {
        if (set_training == 0)
          first_iteration_array_set_0[i] = temporary_memory[i];
        else if (set_training == 1)
          first_iteration_array_set_1[i] = temporary_memory[i];
        else if (set_training == 2)
          first_iteration_array_set_2[i] = temporary_memory[i];
        else if (set_training == 3)
          first_iteration_array_set_3[i] = temporary_memory[i];
      }
    }

#define NUMBER_BLOCKS 2
    uintptr_t iteration;

    
    
    
    iteration = iteration_p;

    int round_retry = 0;

    uintptr_t Intermediate_Hash[5];
    for (int itr = 0; itr < iteration; itr++)
      {
        uintptr_t number_blocks = NUMBER_BLOCKS;
        if (!good_accuracy_found) {
          number_blocks = 1;
        }
      
        uint8_t messageBlock[NUMBER_BLOCKS * 80];
        for (int z = 0; z < NUMBER_BLOCKS * 80; z++) {
          messageBlock[z] = rand() & 0xff;
        }
        

        for (uintptr_t block = 0; block < number_blocks; block++)
        {
          uint8_t messageBlock_local[80];
        for (int z = 80 * block; z < (block + 1) * 80; z++) {
          messageBlock_local[z] = messageBlock[z];
        }
        
        uint32_t W_scheduled[80];
        
    for(int t = 0; t < 16; t++)
    {
        W_scheduled[t] = messageBlock_local[t * 4] << 24;
        W_scheduled[t] |= messageBlock_local[t * 4 + 1] << 16;
        W_scheduled[t] |= messageBlock_local[t * 4 + 2] << 8;
        W_scheduled[t] |= messageBlock_local[t * 4 + 3];
    }
    
    for(int t = 16; t < 80; t++)
    {
       W_scheduled[t] = SHA1CircularShift(1,W_scheduled[t-3] ^ W_scheduled[t-8] ^ W_scheduled[t-14] ^ W_scheduled[t-16]);
    }
    
    mfence();
    lfence();


        uint32_t A_start;
        uint32_t B_start;
        uint32_t C_start;
        uint32_t D_start;
        uint32_t E_start;

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

          
          
          
          
          

          
          
          
          
          
          
          
          

          uint32_t A_read[NUMBER_VOTING];
          uint32_t B_read[NUMBER_VOTING];
          uint32_t C_read[NUMBER_VOTING];
          uint32_t D_read[NUMBER_VOTING];
          uint32_t E_read[NUMBER_VOTING];

          
          int k;
          int number_rounds;

          if (!good_accuracy_found)
          {
            k = set_training * 20;
          }
          else
          {
            k = 0;
          }

          if (!good_accuracy_found)
            number_rounds = set_training * 20 + 1;
          else
            number_rounds = TEST_ROUND;

          double total_retries_required = 0;

          for (; k < number_rounds; k++)
          {

            if (!good_accuracy_found)
            {
              mfence();
              lfence();
              A_start = rand();
              B_start = rand();
              C_start = rand();
              D_start = rand();
              E_start = rand();

              A_arch = A_start;
              B_arch = B_start;
              C_arch = C_start;
              D_arch = D_start;
              E_arch = E_start;
              mfence();
              lfence();
            }

            uint32_t W_arch = W_scheduled[k];
            if (!round_retry)
            {
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

            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            

            
            
            
            
            
            

            int voting_number;
            if (!good_accuracy_found)
              voting_number = 1;
            else
              voting_number = NUMBER_VOTING;

            for (int voting_round = 0; voting_round < voting_number; voting_round++)
            {
              for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
              {
                if (good_accuracy_found)
                {
                  if (k <= 19)
                    temporary_memory[i] = first_iteration_array_set_0[i];
                  else if (k <= 39)
                    temporary_memory[i] = first_iteration_array_set_1[i];
                  else if (k <= 59)
                    temporary_memory[i] = first_iteration_array_set_2[i];
                  else if (k <= 79)
                    temporary_memory[i] = first_iteration_array_set_3[i];
                }
                else
                {
                  if (set_training == 0)
                    temporary_memory[i] = first_iteration_array_set_0[i];
                  else if (set_training == 1)
                    temporary_memory[i] = first_iteration_array_set_1[i];
                  else if (set_training == 2)
                    temporary_memory[i] = first_iteration_array_set_2[i];
                  else if (set_training == 3)
                    temporary_memory[i] = first_iteration_array_set_3[i];
                }
              }

              if (good_accuracy_found)
              {
                
                
                
                mfence();
                lfence();
                pht_nuke();
                pht_nuke();
                
                
              }
              
              

              if (good_accuracy_found)
              {
                mfence();
                lfence();
                trash = sha1_decoy(trash, 25);
              }

              mfence();
              lfence();

              if (k == 0 && good_accuracy_found)
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

                _32BitValueToCacheState(A_start, A1, A2, NULL);

                
                
                
                
                
                
                

                
                
                
                
                
                
                
                
                

                _32BitValueToCacheState(B_start, B1, B2, B3);
                _32BitValueToCacheState(C_start, C1, C2, NULL);
                _32BitValueToCacheState(D_start, D1, D2, NULL);
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

                uintptr_t E_RESULT = D2;
                uintptr_t D_RESULT = C2;
                LeftShift(B3, B3, 2);
                uintptr_t C_RESULT = B3;
                uintptr_t B_RESULT = A2;

                mfence();
                lfence();

                trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(B_RESULT, &B_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(C_RESULT, &C_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(D_RESULT, &D_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(E_RESULT, &E_read[voting_round], trash);

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

              else if (k <= 39)
              {
                mfence();
                lfence();
                uintptr_t A1[WORDSIZE];
                uintptr_t A2[WORDSIZE];

                uintptr_t B1[WORDSIZE];
                uintptr_t B2[WORDSIZE];

                uintptr_t C1[WORDSIZE];
                uintptr_t C2[WORDSIZE];

                uintptr_t D1[WORDSIZE];
                uintptr_t D2[WORDSIZE];

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
                  A2[i] = get_temporary_memory();
                  B1[i] = get_temporary_memory();
                  B2[i] = get_temporary_memory();
                  C1[i] = get_temporary_memory();
                  C2[i] = get_temporary_memory();
                  D1[i] = get_temporary_memory();
                  D2[i] = get_temporary_memory();
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
                _32BitValueToCacheState(A_start, A1, A2, NULL);
                _32BitValueToCacheState(B_start, B1, B2, NULL);
                _32BitValueToCacheState(C_start, C1, C2, NULL);
                _32BitValueToCacheState(D_start, D1, D2, NULL);
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

                uintptr_t E_RESULT = D2;
                uintptr_t D_RESULT = C2;
                LeftShift(B2, B2, 2);
                uintptr_t C_RESULT = B2;
                uintptr_t B_RESULT = A2;

                mfence();
                lfence();

                trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(B_RESULT, &B_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(C_RESULT, &C_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(D_RESULT, &D_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(E_RESULT, &E_read[voting_round], trash);

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
                  return_temporary_memory(D2[i]);
                  return_temporary_memory(D1[i]);
                  return_temporary_memory(C2[i]);
                  return_temporary_memory(C1[i]);
                  return_temporary_memory(B2[i]);
                  return_temporary_memory(B1[i]);
                  return_temporary_memory(A2[i]);
                  return_temporary_memory(A1[i]);
                }
              }

              else if (k <= 59)
              {

                mfence();
                lfence();
                uintptr_t A1[WORDSIZE];
                uintptr_t A2[WORDSIZE];
                uintptr_t B1[WORDSIZE];
                uintptr_t B2[WORDSIZE];
                uintptr_t B3[WORDSIZE];
                uintptr_t C1[WORDSIZE];
                uintptr_t C2[WORDSIZE];
                uintptr_t C3[WORDSIZE];
                uintptr_t D1[WORDSIZE];
                uintptr_t D2[WORDSIZE];
                uintptr_t D3[WORDSIZE];
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
                  A2[i] = get_temporary_memory();

                  B1[i] = get_temporary_memory();
                  B2[i] = get_temporary_memory();
                  B3[i] = get_temporary_memory();

                  C1[i] = get_temporary_memory();
                  C2[i] = get_temporary_memory();
                  C3[i] = get_temporary_memory();

                  D1[i] = get_temporary_memory();
                  D2[i] = get_temporary_memory();
                  D3[i] = get_temporary_memory();
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

                

                _32BitValueToCacheState(A_start, A1, A2, NULL);
                _32BitValueToCacheState(B_start, B1, B2, B3);
                _32BitValueToCacheState(C_start, C1, C2, C3);
                _32BitValueToCacheState(D_start, D1, D2, D3);
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

                
                
                
                
                

                uintptr_t E_RESULT = D3;
                uintptr_t D_RESULT = C3;
                LeftShift(B3, B3, 2);
                uintptr_t C_RESULT = B3;
                uintptr_t B_RESULT = A2;

                trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(B_RESULT, &B_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(C_RESULT, &C_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(D_RESULT, &D_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(E_RESULT, &E_read[voting_round], trash);

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
                  return_temporary_memory(D3[i]);
                  return_temporary_memory(D2[i]);
                  return_temporary_memory(D1[i]);
                  return_temporary_memory(C3[i]);
                  return_temporary_memory(C2[i]);
                  return_temporary_memory(C1[i]);
                  return_temporary_memory(B3[i]);
                  return_temporary_memory(B2[i]);
                  return_temporary_memory(B1[i]);
                  return_temporary_memory(A2[i]);
                  return_temporary_memory(A1[i]);
                }

                
                
                
                
                
              }

              else if (k <= 79)
              {
                mfence();
                lfence();
                uintptr_t A1[WORDSIZE];
                uintptr_t A2[WORDSIZE];

                uintptr_t B1[WORDSIZE];
                uintptr_t B2[WORDSIZE];

                uintptr_t C1[WORDSIZE];
                uintptr_t C2[WORDSIZE];

                uintptr_t D1[WORDSIZE];
                uintptr_t D2[WORDSIZE];

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
                  A2[i] = get_temporary_memory();
                  B1[i] = get_temporary_memory();
                  B2[i] = get_temporary_memory();
                  C1[i] = get_temporary_memory();
                  C2[i] = get_temporary_memory();
                  D1[i] = get_temporary_memory();
                  D2[i] = get_temporary_memory();
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
                _32BitValueToCacheState(A_start, A1, A2, NULL);
                _32BitValueToCacheState(B_start, B1, B2, NULL);
                _32BitValueToCacheState(C_start, C1, C2, NULL);
                _32BitValueToCacheState(D_start, D1, D2, NULL);
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

                uintptr_t E_RESULT = D2;
                uintptr_t D_RESULT = C2;
                LeftShift(B2, B2, 2);
                uintptr_t C_RESULT = B2;
                uintptr_t B_RESULT = A2;

                mfence();
                lfence();

                trash = _cacheStateTo32BitValue(A_RESULT, &A_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(B_RESULT, &B_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(C_RESULT, &C_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(D_RESULT, &D_read[voting_round], trash);
                trash = _cacheStateTo32BitValue(E_RESULT, &E_read[voting_round], trash);

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
                  return_temporary_memory(D2[i]);
                  return_temporary_memory(D1[i]);
                  return_temporary_memory(C2[i]);
                  return_temporary_memory(C1[i]);
                  return_temporary_memory(B2[i]);
                  return_temporary_memory(B1[i]);
                  return_temporary_memory(A2[i]);
                  return_temporary_memory(A1[i]);
                }
              }
            }

            uintptr_t A_start_new = 0;
            uintptr_t B_start_new = 0;
            uintptr_t C_start_new = 0;
            uintptr_t D_start_new = 0;
            uintptr_t E_start_new = 0;

            get_majority_all(A_read, B_read, C_read, D_read, E_read,
                             &A_start_new, &B_start_new, &C_start_new, &D_start_new, &E_start_new, voting_number);

            if (good_accuracy_found)
            {
              printf("Round %d / %d:\n", k, TEST_ROUND);
              printf("A READ: %lx\n", A_start_new);
              printf("A_arch_result = %x\n", A_arch_result);
              print_binary(A_start_new);
              print_binary(A_arch_result);
              if (A_start_new == A_arch_result)
              {
                printf("SAME!\n");
              }
              printf("B READ: %lx\n", B_start_new);
              printf("B_arch_result = %x\n", B_arch_result);
              print_binary(B_start_new);
              print_binary(B_arch_result);
              printf("C READ: %lx\n", C_start_new);
              printf("C_arch_result = %x\n", C_arch_result);
              print_binary(C_start_new);
              print_binary(C_arch_result);
              printf("D READ: %lx\n", D_start_new);
              printf("D_arch_result = %x\n", D_arch_result);
              print_binary(D_start_new);
              print_binary(D_arch_result);
              printf("E READ: %lx\n", E_start_new);
              printf("E_arch_result = %x\n", E_arch_result);
              print_binary(E_start_new);
              print_binary(E_arch_result);

              if (!(A_start_new == A_arch_result &&
                    B_start_new == B_arch_result &&
                    C_start_new == C_arch_result &&
                    D_start_new == D_arch_result &&
                    E_start_new == E_arch_result))
              {
                
                total_retries_required += voting_number;
                
                round_retry = 1;
                k--;
                
              }
              else
              {
                A_start = A_start_new;
                B_start = B_start_new;
                C_start = C_start_new;
                D_start = D_start_new;
                E_start = E_start_new;
              }
            }
            else
            {
              A_start = A_start_new;
              B_start = B_start_new;
              C_start = C_start_new;
              D_start = D_start_new;
              E_start = E_start_new;
            }
          }

          
          
          
          
          
          

          
          
          
          
          

          if (!good_accuracy_found)
          {
            if (A_start == A_arch_result &&
                B_start == B_arch_result &&
                C_start == C_arch_result &&
                D_start == D_arch_result &&
                E_start == E_arch_result)
            {
              correct++;
            }
            else
            {
              incorrect++;
            }
          }
          else
          {

            if ((int)total_retries_required <= 0)
            {
              correct++;
            }
            else
            {
              incorrect++;
            }
            printf("Total runs = %lf\n", (total_retries_required / (double)TEST_ROUND) + (double)NUMBER_VOTING);
          }

          


        if (good_accuracy_found) {


        mfence();
        lfence();

        uintptr_t A_in[WORDSIZE];
        uintptr_t B_in[WORDSIZE];
        uintptr_t C_in[WORDSIZE];
        uintptr_t D_in[WORDSIZE];
        uintptr_t E_in[WORDSIZE];

        uintptr_t Intermediate_Hash_0_in[WORDSIZE];
        uintptr_t Intermediate_Hash_1_in[WORDSIZE];
        uintptr_t Intermediate_Hash_2_in[WORDSIZE];
        uintptr_t Intermediate_Hash_3_in[WORDSIZE];
        uintptr_t Intermediate_Hash_4_in[WORDSIZE];

        uintptr_t Intermediate_Hash_0_out[WORDSIZE];
        uintptr_t Intermediate_Hash_1_out[WORDSIZE];
        uintptr_t Intermediate_Hash_2_out[WORDSIZE];
        uintptr_t Intermediate_Hash_3_out[WORDSIZE];
        uintptr_t Intermediate_Hash_4_out[WORDSIZE];

        uint32_t new_intermediate_hash_0 = 0;
        uint32_t new_intermediate_hash_1 = 0;
        uint32_t new_intermediate_hash_2 = 0;
        uint32_t new_intermediate_hash_3 = 0;
        uint32_t new_intermediate_hash_4 = 0;

        for (uintptr_t i = 0; i < WORDSIZE; i++)
        {
          A_in[i] = get_temporary_memory();
          B_in[i] = get_temporary_memory();
          C_in[i] = get_temporary_memory();
          D_in[i] = get_temporary_memory();
          E_in[i] = get_temporary_memory();

          Intermediate_Hash_0_in[i] = get_temporary_memory();
          Intermediate_Hash_1_in[i] = get_temporary_memory();
          Intermediate_Hash_2_in[i] = get_temporary_memory();
          Intermediate_Hash_3_in[i] = get_temporary_memory();
          Intermediate_Hash_4_in[i] = get_temporary_memory();

          Intermediate_Hash_0_out[i] = get_temporary_memory();
          Intermediate_Hash_1_out[i] = get_temporary_memory();
          Intermediate_Hash_2_out[i] = get_temporary_memory();
          Intermediate_Hash_3_out[i] = get_temporary_memory();
          Intermediate_Hash_4_out[i] = get_temporary_memory();
        }

        mfence();
        lfence();

        _32BitValueToCacheState(Intermediate_Hash[0], Intermediate_Hash_0_in, NULL, NULL);
        _32BitValueToCacheState(Intermediate_Hash[1], Intermediate_Hash_1_in, NULL, NULL);
        _32BitValueToCacheState(Intermediate_Hash[2], Intermediate_Hash_2_in, NULL, NULL);
        _32BitValueToCacheState(Intermediate_Hash[3], Intermediate_Hash_3_in, NULL, NULL);
        _32BitValueToCacheState(Intermediate_Hash[4], Intermediate_Hash_4_in, NULL, NULL);

        _32BitValueToCacheState(A_start, A_in, NULL, NULL);
        _32BitValueToCacheState(B_start, B_in, NULL, NULL);
        _32BitValueToCacheState(C_start, C_in, NULL, NULL);
        _32BitValueToCacheState(D_start, D_in, NULL, NULL);
        _32BitValueToCacheState(E_start, E_in, NULL, NULL);

        mfence();
        lfence();

        _32BitAdder_impl(Intermediate_Hash_0_in, A_in, Intermediate_Hash_0_out, WORDSIZE, trash);

        mfence();
        lfence();

        _32BitAdder_impl(Intermediate_Hash_1_in, B_in, Intermediate_Hash_1_out, WORDSIZE, trash);

        mfence();
        lfence();

        _32BitAdder_impl(Intermediate_Hash_2_in, C_in, Intermediate_Hash_2_out, WORDSIZE, trash);

        mfence();
        lfence();

        _32BitAdder_impl(Intermediate_Hash_3_in, D_in, Intermediate_Hash_3_out, WORDSIZE, trash);

        mfence();
        lfence();

        _32BitAdder_impl(Intermediate_Hash_4_in, E_in, Intermediate_Hash_4_out, WORDSIZE, trash);

        mfence();
        lfence();
        

        _cacheStateTo32BitValue(Intermediate_Hash_0_out, &new_intermediate_hash_0, trash);
        mfence();
        lfence();
        _cacheStateTo32BitValue(Intermediate_Hash_1_out, &new_intermediate_hash_1, trash);
        mfence();
        lfence();
        _cacheStateTo32BitValue(Intermediate_Hash_2_out, &new_intermediate_hash_2, trash);
        mfence();
        lfence();
        _cacheStateTo32BitValue(Intermediate_Hash_3_out, &new_intermediate_hash_3, trash);
        mfence();
        lfence();
        _cacheStateTo32BitValue(Intermediate_Hash_4_out, &new_intermediate_hash_4, trash);
        mfence();
        lfence();


        for (intptr_t i = WORDSIZE - 1; i >= 0; i--)
        {
          return_temporary_memory(Intermediate_Hash_4_out[i]);
          return_temporary_memory(Intermediate_Hash_3_out[i]);
          return_temporary_memory(Intermediate_Hash_3_out[i]);
          return_temporary_memory(Intermediate_Hash_2_out[i]);
          return_temporary_memory(Intermediate_Hash_1_out[i]);
          return_temporary_memory(Intermediate_Hash_0_out[i]);

          return_temporary_memory(Intermediate_Hash_4_in[i]);
          return_temporary_memory(Intermediate_Hash_3_in[i]);
          return_temporary_memory(Intermediate_Hash_3_in[i]);
          return_temporary_memory(Intermediate_Hash_2_in[i]);
          return_temporary_memory(Intermediate_Hash_1_in[i]);
          return_temporary_memory(Intermediate_Hash_0_in[i]);

          return_temporary_memory(E_in[i]);
          return_temporary_memory(D_in[i]);
          return_temporary_memory(C_in[i]);
          return_temporary_memory(B_in[i]);
          return_temporary_memory(A_in[i]);
        }

        Intermediate_Hash[1] += (uintptr_t)A_start;
        Intermediate_Hash[2] += (uintptr_t)B_start;
        Intermediate_Hash[3] += (uintptr_t)C_start;
        Intermediate_Hash[4] += (uintptr_t)D_start;
        Intermediate_Hash[5] += (uintptr_t)E_start;

        printf("##### BLOCK %d #####\n", block);

        if (new_intermediate_hash_0 == A_start &&
            new_intermediate_hash_1 == B_start &&
            new_intermediate_hash_2 == C_start &&
            new_intermediate_hash_3 == D_start &&
            new_intermediate_hash_4 == E_start) {
          printf("Block correct!  -- retries = %lf\n", total_retries_required);
        } else {
          printf("Block incorrect!\n");
        }




        }
        }



      }
#define TARGET_ACCURACY 0.98 
                             

      const double accuracy = (double)correct / (double)(incorrect + correct);
      printf("SHA1 Accuracy : %d / %d (%lf) \n", correct, correct + incorrect,
             (double)correct / (double)(correct + incorrect));
      if (good_accuracy_found || accuracy >= TARGET_ACCURACY)
      {
        printf("SHA1 Accuracy : %d / %d (%lf) \n", correct, correct + incorrect,
               (double)correct / (double)(correct + incorrect));
      }

#define FAIL_TO_FIND_THRESHOLD 1000
      if (accuracy < TARGET_ACCURACY && !good_accuracy_found)
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
          set_training++;
          if (set_training >= 4)
          {
            good_accuracy_found = true;
          }
          
          
          
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
