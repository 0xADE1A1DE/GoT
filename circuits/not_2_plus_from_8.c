#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t  not_2_plus_from_8_impl(uintptr_t addresses[8], uintptr_t output, bool wet_run, uintptr_t trash) {
  
#pragma GCC unroll 300
  for (int i = 0; i < 15; i++)
  {
    for (int i = 0; i < 20; i++)
    {
      asm volatile("");
    }
  }

  register uintptr_t val1 = FORCE_READ(addresses[0], trash);
  register uintptr_t val2 = FORCE_READ(addresses[1], trash);
  register uintptr_t val3 = FORCE_READ(addresses[2], trash);
  register uintptr_t val4 = FORCE_READ(addresses[3], trash);
  register uintptr_t val5 = FORCE_READ(addresses[4], trash);
  register uintptr_t val6 = FORCE_READ(addresses[5], trash);
  register uintptr_t val7 = FORCE_READ(addresses[6], trash);
  register uintptr_t val8 = FORCE_READ(addresses[7], trash);
  
  volatile uintptr_t new_trash1 = val1 | val2;
  volatile uintptr_t new_trash2 = val1 | val3;
  volatile uintptr_t new_trash3 = val1 | val4;
  volatile uintptr_t new_trash4 = val1 | val5;
  volatile uintptr_t new_trash5 = val1 | val6;
  volatile uintptr_t new_trash6 = val1 | val7;
  volatile uintptr_t new_trash7 = val1 | val8;
  volatile uintptr_t new_trash8 = val2 | val3;
  volatile uintptr_t new_trash9 = val2 | val4;
  volatile uintptr_t new_trash10 = val2 | val5;
  volatile uintptr_t new_trash11 = val2 | val6;
  volatile uintptr_t new_trash12 = val2 | val7;
  volatile uintptr_t new_trash13 = val2 | val8;
  volatile uintptr_t new_trash14 = val3 | val4;
  volatile uintptr_t new_trash15 = val3 | val5;
  volatile uintptr_t new_trash16 = val3 | val6;
  volatile uintptr_t new_trash17 = val3 | val7;
  volatile uintptr_t new_trash18 = val3 | val8;
  volatile uintptr_t new_trash19 = val4 | val5;
  volatile uintptr_t new_trash20 = val4 | val6;
  volatile uintptr_t new_trash21 = val4 | val7;
  volatile uintptr_t new_trash22 = val4 | val8;
  volatile uintptr_t new_trash23 = val5 | val6;
  volatile uintptr_t new_trash24 = val5 | val7;  
  volatile uintptr_t new_trash25 = val5 | val8;
  volatile uintptr_t new_trash26 = val6 | val7;
  volatile uintptr_t new_trash27 = val6 | val8;
  volatile uintptr_t new_trash28 = val7 | val8;

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
  
  if (wet_run == (new_trash7 != 0xbaaaaad))
  {
    return new_trash7;
  }

  if (wet_run == (new_trash8 != 0xbaaaaad))
  {
    return new_trash8;
  }

  if (wet_run == (new_trash9 != 0xbaaaaad))
  {
    return new_trash9;
  }
  
  if (wet_run == (new_trash10 != 0xbaaaaad))
  {
    return new_trash10;
  }
  
  if (wet_run == (new_trash11 != 0xbaaaaad))
  {
    return new_trash11;
  }
  
  if (wet_run == (new_trash12 != 0xbaaaaad))
  {
    return new_trash12;
  }

  if (wet_run == (new_trash13 != 0xbaaaaad))
  {
    return new_trash13;
  }

  if (wet_run == (new_trash14 != 0xbaaaaad))
  {
    return new_trash14;
  }
  
  if (wet_run == (new_trash15 != 0xbaaaaad))
  {
    return new_trash15;
  }

  if (wet_run == (new_trash16 != 0xbaaaaad))
  {
    return new_trash16;
  }

  if (wet_run == (new_trash17 != 0xbaaaaad))
  {
    return new_trash17;
  }

  if (wet_run == (new_trash18 != 0xbaaaaad))
  {
    return new_trash18;
  }

  if (wet_run == (new_trash19 != 0xbaaaaad))
  {
    return new_trash19;
  }
  
  if (wet_run == (new_trash20 != 0xbaaaaad))
  {
    return new_trash20;
  }
  
  if (wet_run == (new_trash21 != 0xbaaaaad))
  {
    return new_trash21;
  }
  
  if (wet_run == (new_trash23 != 0xbaaaaad))
  {
    return new_trash23;
  }
  
  if (wet_run == (new_trash24 != 0xbaaaaad))
  {
    return new_trash24;
  }

  if (wet_run == (new_trash25 != 0xbaaaaad))
  {
    return new_trash25;
  }
  
  if (wet_run == (new_trash26 != 0xbaaaaad))
  {
    return new_trash26;
  }
  
  if (wet_run == (new_trash27 != 0xbaaaaad))
  {
    return new_trash27;
  }
  
  if (wet_run == (new_trash28 != 0xbaaaaad))
  {
    return new_trash28;
  }

 
  asm volatile("");
  if (!wet_run)
    return trash;

  
  
  
  
  
  
  

  trash += *(uintptr_t *)(output + trash);
  return trash + new_trash1 + new_trash2 + new_trash3 + new_trash4 + new_trash5 + new_trash6 
    + new_trash7 + new_trash8 + new_trash9 + new_trash10
    + new_trash11 + new_trash12 + new_trash13 +
    new_trash14 + new_trash15 + new_trash16 + new_trash17 + new_trash18 + new_trash19 + new_trash20 +
    new_trash21 + new_trash22 + new_trash23 + new_trash24 + new_trash25 + new_trash26 + new_trash27 +
    new_trash28;

}

uintptr_t not_2_plus_from_8_gate(uintptr_t input[8], uintptr_t output, uintptr_t trash)
{
  gate_metadata __attribute__((aligned(64))) fake = {0};
  uintptr_t fake_arr[8] = {&fake, &fake, &fake, &fake, &fake, &fake, &fake, &fake};
  
  
  

  trash = not_2_plus_from_8_impl(&fake_arr, &fake, false, trash);
  trash = not_2_plus_from_8_impl(&fake_arr, &fake, false, trash);
  trash = not_2_plus_from_8_impl(&fake_arr, &fake, false, trash);
  trash = not_2_plus_from_8_impl(&fake_arr, &fake, false, trash);
  trash = not_2_plus_from_8_impl(&fake_arr, &fake, false, trash);


  
  trash = not_2_plus_from_8_impl(input, output, true, trash);

  return trash;
}


uintptr_t not_2_plus_from_8_test_rec(uintptr_t state[8], uintptr_t counter, uintptr_t trash) {
  if (counter == 8) {
    uintptr_t input[8];
    for (uintptr_t i = 0; i < 8; i++) {
      input[i] = get_temporary_memory();
    }
    
    uintptr_t output = get_temporary_memory();
    uintptr_t output_read;
    
    for (uintptr_t i = 0; i < 8; i++) {
      printf("%ld ", state[i]);
    }
    printf(": ");
    
    clflush(output);
    
    for (uintptr_t i = 0; i < 8; i++) {
      clflush(input[i]);
      mfence();
      lfence();
      if (state[i])
        trash = FORCE_READ(input[i], trash);
      mfence();
      lfence();
    }
    
    trash = not_2_plus_from_8_gate(input, output, trash);
    
    mfence();
    lfence();
    
    trash = read_addr(output, &output_read, trash);
    printf("%ld\n", output_read);
    

    return_temporary_memory(output);
    for (intptr_t i = 7; i >= 0; i--) {
      return_temporary_memory(input[i]);
    }
    
    return trash;
  } else {
    state[counter] = 0;
    trash += not_2_plus_from_8_test_rec(state, counter + 1, trash);
    state[counter] = 1;
    trash += not_2_plus_from_8_test_rec(state, counter + 1, trash);
    
    return trash;
  }
}

uintptr_t test_not_2_from_8(uintptr_t trash) {
  uintptr_t state[8] = {0};
  
  return not_2_plus_from_8_test_rec(state, 0, trash);
}