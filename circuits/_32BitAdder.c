#include <stdlib.h>
#include <time.h>

#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

#define BITSIZE 32

extern uintptr_t temporary_memory[TEMP_MEMORY_SIZE];
extern void *memory;

uintptr_t _32BitAdder_impl(uintptr_t *a, uintptr_t *b, uintptr_t *out, uintptr_t size, uintptr_t trash)
{
  uintptr_t carry = get_temporary_memory();
  clflush(carry);
  mfence();
  lfence();


  for (uintptr_t i = 0; i < BITSIZE; i++)
  {
    uintptr_t newCarry = get_temporary_memory();
    
    
    clflush(newCarry);
    mfence();
    lfence();

    trash = full_adder_impl(a[i], b[i], carry, out[i], newCarry, trash);

    mfence();
    lfence();

    return_temporary_memory(carry);
    carry = newCarry;
  }

  return_temporary_memory(carry);
  return trash;
}

static void get_expected(uintptr_t a[], uintptr_t b[], uintptr_t expected[])
{
  uintptr_t carry = 0;
  for (int i = 0; i < BITSIZE; i++)
  {
    uintptr_t sum = carry + a[i] + b[i];
    expected[i] = sum & 1;
    carry = sum >> 1;
  }
}

#define TARGET_SUCCESS_COUNTS 10

uintptr_t test_32BitAdder_accuracy(uintptr_t trash, uintptr_t iteration)
{
  srand(time(NULL));

  uintptr_t target_accuracy_success = 0;

  while (true)
  {
    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    uintptr_t a[BITSIZE];
    uintptr_t b[BITSIZE];
    uintptr_t out[BITSIZE];

    uintptr_t aArch[BITSIZE];
    uintptr_t bArch[BITSIZE];
    uintptr_t expected[BITSIZE];

    for (int i = 0; i < iteration; i++)
    {

      for (int i = 0; i < BITSIZE; i++)
      {
        a[i] = get_temporary_memory();
        b[i] = get_temporary_memory();
        out[i] = get_temporary_memory();

        aArch[i] = ((rand() % 100) < 50);
        bArch[i] = ((rand() % 100) < 50);
      }
      get_expected(aArch, bArch, expected);

      for (int i = 0; i < BITSIZE; i++)
      {
        mfence();
        lfence();
        clflush(a[i]);
        clflush(b[i]);
        clflush(out[i]);
        mfence();
        lfence();
      }

      for (int i = 0; i < BITSIZE; i++)
      {
        if (aArch[i])
          trash = FORCE_READ(a[i], trash);
        if (bArch[i])
          trash = FORCE_READ(b[i], trash);
      }


      
      
      
      
      
      

      
      
      
      
      
      

      
      
      
      
      
      


      mfence();
      lfence();

      trash = _32BitAdder_impl(a, b, out, BITSIZE, trash);

      mfence();
      lfence();

      uintptr_t read_out;

      
      bool correct_flag = true;
      for (int i = 0; i < BITSIZE; i++)
      {
        trash = read_addr(out[i], &read_out, trash);
        if (read_out != expected[i])
        {
          correct_flag = false;
        }
        
      }
      
      if (correct_flag) {
        correct++;
        
      }
      else {
        incorrect++;
        
      }
      for (int i = 0; i < BITSIZE; i++)
      {
        return_temporary_memory(out[i]);
        return_temporary_memory(b[i]);
        return_temporary_memory(a[i]);
      }
    }
    printf("32 Bits Adder Accuracy : %d / %d\n", correct, correct + incorrect);

    const double accuracy = (double)correct / (double)(incorrect + correct);

    if (accuracy < 0.7)
    {
      printf("accuracy fell short, randomising temp memory!\n");
      
      target_accuracy_success = 0;
      randomise_temp_memory();
      
      
      
      
      
      
      
    }
    else
    {
      target_accuracy_success++;
      if (target_accuracy_success >= TARGET_SUCCESS_COUNTS)
      {
        printf("target accuract reached! continuing test\n");
        printf("temp memory: {");
        for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
        {
          printf("%ld, ", (temporary_memory[i] - (uintptr_t)memory) / CACHE_LINE_SIZE);
        }
        printf("}\n");
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
                                       
