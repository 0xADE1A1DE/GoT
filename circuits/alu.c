#include <stdlib.h>
#include <stdio.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

#ifdef GATES_STATS
  #include <sys/time.h>
#endif

#define BIT_WIDTH 4

extern uintptr_t temporary_memory[TEMP_MEMORY_SIZE];
extern uintptr_t available_temporary_memory;

extern intptr_t prefetcher_enabled;

void adder_4_bit_simulate(uintptr_t a[BIT_WIDTH], uintptr_t b[BIT_WIDTH], uintptr_t result[BIT_WIDTH])
{
  uintptr_t carry = 0;
  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    uintptr_t sum = carry + a[i] + b[i];
    result[i] = sum & 1;
    carry = sum >> 1;
  }
  return;
}

void alu_simulate(uintptr_t x_p[BIT_WIDTH], uintptr_t y_p[BIT_WIDTH], uintptr_t zx,
                  uintptr_t nx, uintptr_t zy, uintptr_t ny, uintptr_t f, uintptr_t no,
                  uintptr_t out[BIT_WIDTH], uintptr_t *zr, uintptr_t *ng)
{
  uintptr_t x[BIT_WIDTH];
  uintptr_t y[BIT_WIDTH];
  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    x[i] = x_p[i];
    y[i] = y_p[i];
  }
  if (zx)
  {
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      x[i] = 0;
    }
  }

  if (zy)
  {
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      y[i] = 0;
    }
  }

  if (nx)
  {
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      x[i] = !x[i];
    }
  }

  if (ny)
  {
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      y[i] = !y[i];
    }
  }

  if (f)
  {
    adder_4_bit_simulate(x, y, out);
  }
  else
  {
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      out[i] = x[i] & y[i];
    }
  }

  if (no)
  {
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      out[i] = !out[i];
    }
  }

  *ng = out[BIT_WIDTH - 1];

  uintptr_t sum = 0;
  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    sum += out[i];
  }

  *zr = !sum;

  return;
}

uintptr_t alu_impl(uintptr_t x[BIT_WIDTH], uintptr_t y[BIT_WIDTH], uintptr_t zx,
                   uintptr_t nx, uintptr_t zy, uintptr_t ny, uintptr_t f, uintptr_t no,
                   uintptr_t out[BIT_WIDTH], uintptr_t trash)
{
  uintptr_t zx_copies[BIT_WIDTH];
  uintptr_t zy_copies[BIT_WIDTH];

  uintptr_t nx_copies[BIT_WIDTH];
  uintptr_t ny_copies[BIT_WIDTH];

  uintptr_t xmux_out[BIT_WIDTH];
  uintptr_t ymux_out[BIT_WIDTH];

  uintptr_t x1_copies_1[BIT_WIDTH];
  uintptr_t x1_copies_2[BIT_WIDTH];

  uintptr_t y1_copies_1[BIT_WIDTH];
  uintptr_t y1_copies_2[BIT_WIDTH];

  uintptr_t x2_copies_1[BIT_WIDTH];
  uintptr_t x2_copies_2[BIT_WIDTH];

  uintptr_t y2_copies_1[BIT_WIDTH];
  uintptr_t y2_copies_2[BIT_WIDTH];

  uintptr_t xnot_out[BIT_WIDTH];
  uintptr_t ynot_out[BIT_WIDTH];

  uintptr_t xnot_mux_out[BIT_WIDTH];
  uintptr_t ynot_mux_out[BIT_WIDTH];

  uintptr_t add_xy[BIT_WIDTH];
  uintptr_t and_xy[BIT_WIDTH];

  uintptr_t f_copies[BIT_WIDTH];

  uintptr_t add_and_mux_out[BIT_WIDTH];

  uintptr_t add_and_copies_1[BIT_WIDTH];
  uintptr_t add_and_copies_2[BIT_WIDTH];

  uintptr_t final_not[BIT_WIDTH];
  uintptr_t no_copies[BIT_WIDTH];

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    zx_copies[i] = get_temporary_memory();
    zy_copies[i] = get_temporary_memory();

    nx_copies[i] = get_temporary_memory();
    ny_copies[i] = get_temporary_memory();

    xmux_out[i] = get_temporary_memory();
    ymux_out[i] = get_temporary_memory();

    x1_copies_1[i] = get_temporary_memory();
    x1_copies_2[i] = get_temporary_memory();

    y1_copies_1[i] = get_temporary_memory();
    y1_copies_2[i] = get_temporary_memory();

    x2_copies_1[i] = get_temporary_memory();
    x2_copies_2[i] = get_temporary_memory();

    y2_copies_1[i] = get_temporary_memory();
    y2_copies_2[i] = get_temporary_memory();

    xnot_out[i] = get_temporary_memory();
    ynot_out[i] = get_temporary_memory();

    xnot_mux_out[i] = get_temporary_memory();
    ynot_mux_out[i] = get_temporary_memory();

    add_xy[i] = get_temporary_memory();
    and_xy[i] = get_temporary_memory();

    f_copies[i] = get_temporary_memory();

    add_and_mux_out[i] = get_temporary_memory();

    add_and_copies_1[i] = get_temporary_memory();
    add_and_copies_2[i] = get_temporary_memory();

    final_not[i] = get_temporary_memory();
    no_copies[i] = get_temporary_memory();

    mfence();
    lfence();

    clflush(zx_copies[i]);
    clflush(zy_copies[i]);

    clflush(nx_copies[i]);
    clflush(ny_copies[i]);

    clflush(xmux_out[i]);
    clflush(ymux_out[i]);

    clflush(x1_copies_1[i]);
    clflush(x1_copies_2[i]);

    clflush(y1_copies_1[i]);
    clflush(y1_copies_2[i]);

    clflush(x2_copies_1[i]);
    clflush(x2_copies_2[i]);

    clflush(y2_copies_1[i]);
    clflush(y2_copies_2[i]);

    clflush(xnot_out[i]);
    clflush(ynot_out[i]);

    clflush(add_xy[i]);
    clflush(and_xy[i]);

    clflush(f_copies[i]);

    clflush(add_and_mux_out[i]);

    clflush(add_and_copies_1[i]);
    clflush(add_and_copies_2[i]);

    clflush(final_not[i]);
    clflush(no_copies[i]);

    mfence();
    lfence();
  }

  uintptr_t always_zero = get_temporary_memory();

  trash = fan_gate_4(zx, zx_copies[0], zx_copies[1],
                     zx_copies[2], zx_copies[3], trash);

  mfence();
  lfence();

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(always_zero);
    clflush(xmux_out[i]);
    mfence();
    lfence();
    trash = mux_impl(x[i], always_zero, zx_copies[i], xmux_out[i], trash);
    mfence();
    lfence();
  }

  trash = fan_gate_4(zy, zy_copies[0], zy_copies[1],
                     zy_copies[2], zy_copies[3], trash);

  mfence();
  lfence();

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(always_zero);
    clflush(ymux_out[i]);
    mfence();
    lfence();
    trash = mux_impl(y[i], always_zero, zy_copies[i], ymux_out[i], trash);
    mfence();
    lfence();
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(x1_copies_1[i]);
    clflush(x1_copies_2[i]);
    mfence();
    lfence();

    trash = fan_gate(xmux_out[i], x1_copies_1[i], x1_copies_2[i], trash);
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(y1_copies_1[i]);
    clflush(y1_copies_2[i]);
    mfence();
    lfence();

    trash = fan_gate(ymux_out[i], y1_copies_1[i], y1_copies_2[i], trash);
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(xnot_out[i]);

    mfence();
    lfence();

    trash = not_fan_gate(x1_copies_1[i], xnot_out[i], trash);
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(ynot_out[i]);

    mfence();
    lfence();

    trash = not_fan_gate(y1_copies_1[i], ynot_out[i], trash);
  }

  mfence();
  lfence();

  clflush(nx_copies[0]);
  clflush(nx_copies[1]);
  clflush(nx_copies[2]);
  clflush(nx_copies[3]);

  mfence();
  lfence();

  trash = fan_gate_4(nx, nx_copies[0], nx_copies[1],
                     nx_copies[2], nx_copies[3], trash);

  mfence();
  lfence();

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(xnot_mux_out[i]);
    mfence();
    lfence();
    trash = mux_impl(x1_copies_2[i], xnot_out[i], nx_copies[i], xnot_mux_out[i], trash);
    mfence();
    lfence();
  }

  clflush(ny_copies[0]);
  clflush(ny_copies[1]);
  clflush(ny_copies[2]);
  clflush(ny_copies[3]);

  mfence();
  lfence();

  trash = fan_gate_4(ny, ny_copies[0], ny_copies[1],
                     ny_copies[2], ny_copies[3], trash);

  mfence();
  lfence();

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(ynot_mux_out[i]);
    mfence();
    lfence();
    trash = mux_impl(y1_copies_2[i], ynot_out[i], ny_copies[i], ynot_mux_out[i], trash);
    mfence();
    lfence();
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(x2_copies_1[i]);
    clflush(x2_copies_2[i]);
    mfence();
    lfence();

    trash = fan_gate(xnot_mux_out[i], x2_copies_1[i], x2_copies_2[i], trash);

    mfence();
    lfence();
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(y2_copies_1[i]);
    clflush(y2_copies_2[i]);
    mfence();
    lfence();

    trash = fan_gate(ynot_mux_out[i], y2_copies_1[i], y2_copies_2[i], trash);

    mfence();
    lfence();
  }

  clflush(add_xy[0]);
  clflush(add_xy[1]);
  clflush(add_xy[2]);
  clflush(add_xy[3]);

  mfence();
  lfence();

  trash = NBitAdder_impl(x2_copies_1, y2_copies_1, add_xy,
                         BIT_WIDTH, trash);

  mfence();
  lfence();

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(and_xy[i]);

    mfence();
    lfence();

    trash = and_fan_gate(x2_copies_2[i], y2_copies_2[i], and_xy[i], trash);
  }

  mfence();
  lfence();

  clflush(f_copies[0]);
  clflush(f_copies[1]);
  clflush(f_copies[2]);
  clflush(f_copies[3]);

  mfence();
  lfence();

  trash = fan_gate_4(f, f_copies[0], f_copies[1],
                     f_copies[2], f_copies[3], trash);

  mfence();
  lfence();

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(add_and_mux_out[i]);
    mfence();
    lfence();
    trash = mux_impl(and_xy[i], add_xy[i], f_copies[i], add_and_mux_out[i], trash);
    mfence();
    lfence();
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(add_and_copies_1[i]);
    clflush(add_and_copies_2[i]);
    mfence();
    lfence();

    trash = fan_gate(add_and_mux_out[i], add_and_copies_1[i], add_and_copies_2[i], trash);

    mfence();
    lfence();
  }

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(final_not[i]);
    mfence();
    lfence();
    trash = not_fan_gate(add_and_copies_1[i], final_not[i], trash);
    mfence();
    lfence();
  }

  mfence();
  lfence();

  clflush(no_copies[0]);
  clflush(no_copies[1]);
  clflush(no_copies[2]);
  clflush(no_copies[3]);

  mfence();
  lfence();

  trash = fan_gate_4(no, no_copies[0], no_copies[1],
                     no_copies[2], no_copies[3], trash);

  mfence();
  lfence();

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    clflush(out[i]);
    mfence();
    lfence();
    trash = mux_impl(add_and_copies_2[i], final_not[i], no_copies[i], out[i], trash);
    mfence();
    lfence();
  }
  return_temporary_memory(always_zero);

  for (intptr_t i = BIT_WIDTH - 1; i >= 0; i--)
  {

    return_temporary_memory(no_copies[i]);
    return_temporary_memory(final_not[i]);

    return_temporary_memory(add_and_copies_2[i]);
    return_temporary_memory(add_and_copies_1[i]);

    return_temporary_memory(add_and_mux_out[i]);

    return_temporary_memory(f_copies[i]);

    return_temporary_memory(and_xy[i]);
    return_temporary_memory(add_xy[i]);

    return_temporary_memory(ynot_mux_out[i]);
    return_temporary_memory(xnot_mux_out[i]);

    return_temporary_memory(ynot_out[i]);
    return_temporary_memory(xnot_out[i]);

    return_temporary_memory(y2_copies_2[i]);
    return_temporary_memory(y2_copies_1[i]);

    return_temporary_memory(x2_copies_2[i]);
    return_temporary_memory(x2_copies_1[i]);

    return_temporary_memory(y1_copies_2[i]);
    return_temporary_memory(y1_copies_1[i]);

    return_temporary_memory(x1_copies_2[i]);
    return_temporary_memory(x1_copies_1[i]);

    return_temporary_memory(ymux_out[i]);
    return_temporary_memory(xmux_out[i]);

    return_temporary_memory(ny_copies[i]);
    return_temporary_memory(nx_copies[i]);

    return_temporary_memory(zy_copies[i]);
    return_temporary_memory(zx_copies[i]);
  }
  return trash;
  
  
}

uintptr_t test_alu(uintptr_t trash)
{
  uintptr_t x[BIT_WIDTH];
  uintptr_t y[BIT_WIDTH];

  uintptr_t zx, nx, zy, ny, f, no;

  uintptr_t out[BIT_WIDTH];

  uintptr_t read_output[BIT_WIDTH];

  for (uintptr_t i = 0; i < BIT_WIDTH; i++)
  {
    x[i] = get_temporary_memory();
    y[i] = get_temporary_memory();
    out[i] = get_temporary_memory();
  }
  zx = get_temporary_memory();
  nx = get_temporary_memory();

  zy = get_temporary_memory();
  ny = get_temporary_memory();

  f = get_temporary_memory();
  no = get_temporary_memory();

  uintptr_t x_arch[][BIT_WIDTH] = {{0, 0, 0, 0}};
  uintptr_t y_arch[][BIT_WIDTH] = {{0, 0, 0, 0}};

  uintptr_t zx_arch[] = {0};
  uintptr_t nx_arch[] = {0};

  uintptr_t zy_arch[] = {0};
  uintptr_t ny_arch[] = {0};

  uintptr_t f_arch[] = {0};

  uintptr_t no_arch[] = {0};

  uintptr_t test_count = sizeof(zy_arch) / sizeof(uintptr_t);

  for (uintptr_t ctr = 0; ctr < test_count; ctr++)
  {
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      clflush(x[i]);
      clflush(y[i]);

      mfence();
      lfence();

      if (x_arch[ctr][i])
        trash = FORCE_READ(x[i], trash);

      if (y_arch[ctr][i])
        trash = FORCE_READ(y[i], trash);

      mfence();
      lfence();
    }
    clflush(nx);
    clflush(zx);
    clflush(zy);
    clflush(ny);
    clflush(f);
    clflush(no);
    mfence();
    lfence();

    if (zx_arch[ctr])
      trash = FORCE_READ(zx, trash);

    if (nx_arch[ctr])
      trash = FORCE_READ(nx, trash);

    if (zy_arch[ctr])
      trash = FORCE_READ(ny, trash);

    if (ny_arch[ctr])
      trash = FORCE_READ(ny, trash);

    if (f_arch[ctr])
      trash = FORCE_READ(f, trash);

    if (no_arch[ctr])
      trash = FORCE_READ(no, trash);

    mfence();
    lfence();

    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      clflush(out[i]);

      mfence();
      lfence();
    }

    mfence();
    lfence();

    trash = alu_impl(x, y, zx, nx, zy, ny, f, no, out, trash);

    mfence();
    lfence();

    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      trash = read_addr(out[i], &read_output[i], trash);
    }

    mfence();
    lfence();

    printf("ALU Output: ");
    for (uintptr_t i = 0; i < BIT_WIDTH; i++)
    {
      printf("%ld", read_output[i]);
    }
    printf("\n");
  }

  return_temporary_memory(no);
  return_temporary_memory(f);

  return_temporary_memory(ny);
  return_temporary_memory(zy);

  return_temporary_memory(nx);
  return_temporary_memory(zx);

  for (intptr_t i = BIT_WIDTH - 1; i >= 0; i--)
  {
    return_temporary_memory(out[i]);
    return_temporary_memory(y[i]);
    return_temporary_memory(x[i]);
  }

  return trash;
}


extern uint64_t TARGET_SUCCESS_COUNTS;

#define FAIL_TO_FIND_THRESHOLD 1000

uintptr_t test_alu_accuracy(uintptr_t trash, uintptr_t iteration)
{
  srand(time(NULL));

  uintptr_t target_accuracy_success = 0;
  
  bool good_accuracy_found = false;
  
  uintptr_t failed_to_get_good_address = 0;
  
  uintptr_t first_iteration_array[TEMP_MEMORY_SIZE];

  while (true)
  {

    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    uintptr_t x[BIT_WIDTH];
    uintptr_t y[BIT_WIDTH];

    uintptr_t zx, nx, zy, ny, f, no;

    uintptr_t out[BIT_WIDTH];

    uintptr_t zr, ng;

    uintptr_t read_output[BIT_WIDTH];

    uintptr_t x_arch[BIT_WIDTH];
    uintptr_t y_arch[BIT_WIDTH];

    uintptr_t zx_arch;
    uintptr_t nx_arch;
    uintptr_t zy_arch;
    uintptr_t ny_arch;

    uintptr_t f_arch;

    uintptr_t no_arch;

    uintptr_t expected_output[BIT_WIDTH];
    uintptr_t expected_zr;
    uintptr_t expected_ng;
    
    if (!good_accuracy_found)
    {
      randomise_temp_memory();
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
      {
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
      if (good_accuracy_found)
      {
        for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
        {
          temporary_memory[i] = first_iteration_array[i];
        }
      }
      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        x_arch[i] = rand() & 1;
        y_arch[i] = rand() & 1;
      }
      zx_arch = rand() & 1;
      nx_arch = rand() & 1;
      zy_arch = rand() & 1;
      ny_arch = rand() & 1;
      f_arch = rand() & 1;
      
      no_arch = rand() & 1;

      alu_simulate(x_arch, y_arch, zx_arch, nx_arch, zy_arch, ny_arch,
                   f_arch, no_arch, expected_output, &expected_zr, &expected_ng);

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        x[i] = get_temporary_memory();
        y[i] = get_temporary_memory();
        out[i] = get_temporary_memory();
      }
      zx = get_temporary_memory();
      nx = get_temporary_memory();

      zy = get_temporary_memory();
      ny = get_temporary_memory();

      f = get_temporary_memory();
      no = get_temporary_memory();

      zr = get_temporary_memory();
      ng = get_temporary_memory();

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        clflush(x[i]);
        clflush(y[i]);

        mfence();
        lfence();

        if (x_arch[i])
          trash = FORCE_READ(x[i], trash);

        if (y_arch[i])
          trash = FORCE_READ(y[i], trash);

        mfence();
        lfence();
      }
      clflush(nx);
      clflush(zx);
      clflush(zy);
      clflush(ny);
      clflush(f);
      clflush(no);
      mfence();
      lfence();

      if (zx_arch)
        trash = FORCE_READ(zx, trash);

      if (nx_arch)
        trash = FORCE_READ(nx, trash);

      if (zy_arch)
        trash = FORCE_READ(zy, trash);

      if (ny_arch)
        trash = FORCE_READ(ny, trash);

      if (f_arch)
        trash = FORCE_READ(f, trash);

      if (no_arch)
        trash = FORCE_READ(no, trash);

      mfence();
      lfence();

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        clflush(out[i]);

        mfence();
        lfence();
      }
      clflush(zr);
      clflush(ng);

      #ifdef GATES_STATS
        gettimeofday(&start, NULL);
      #endif
      mfence();
      lfence();
      

      trash = alu_impl(x, y, zx, nx, zy, ny, f, no, out, trash);
      

      mfence();
      lfence();
      
      #ifdef GATES_STATS
        gettimeofday(&stop, NULL);
        circuit_execution_time_us += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
      #endif

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        trash = read_addr(out[i], &read_output[i], trash);
      }

      mfence();
      lfence();

      
      
      
      
      
      

      

      
      
      
      
      
      

      

      bool all_bits_correct = true;

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        if (read_output[i] != expected_output[i])
          all_bits_correct = false;
      }
      correct += all_bits_correct;
      incorrect += !all_bits_correct;

      return_temporary_memory(ng);
      return_temporary_memory(zr);

      return_temporary_memory(no);
      return_temporary_memory(f);

      return_temporary_memory(ny);
      return_temporary_memory(zy);

      return_temporary_memory(nx);
      return_temporary_memory(zx);

      for (intptr_t i = BIT_WIDTH - 1; i >= 0; i--)
      {
        return_temporary_memory(out[i]);
        return_temporary_memory(y[i]);
        return_temporary_memory(x[i]);
      }

    } 
    
    #ifdef GATES_STATS
      printf("Execution time averaged over %ld runs: %lf\n", iteration, circuit_execution_time_us / (double)iteration);
      printf("Gates count: %lf\n", gates_count / (double)iteration);
      printf("Intermediate values: %lf\n", intermediate_values / (double)iteration);
    #endif
    
    
    
    double TARGET_ACCURACY = 0.9;
    
    
    
    const double accuracy = (double)correct / (double)(incorrect + correct);
    if (good_accuracy_found || accuracy >= TARGET_ACCURACY)
      printf("ALU Accuracy : %d / %d (%lf)\n", correct, correct + incorrect,
             accuracy);
      
    if (accuracy < TARGET_ACCURACY && !good_accuracy_found)
    {
      printf("accuracy fell short, randomising temp memory!\n");
      target_accuracy_success = 0;
      failed_to_get_good_address++;
      if (failed_to_get_good_address >= FAIL_TO_FIND_THRESHOLD)
        exit(0);
      
    }
    else
    {
      if (!good_accuracy_found)
      {
        good_accuracy_found = true;

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

uintptr_t test_alu_accuracy_majority(uintptr_t trash, uintptr_t iteration)
{ 
  srand(time(NULL));

  bool good_accuracy_found = false;

  uintptr_t target_accuracy_success = 0;
  
  uintptr_t failed_to_get_good_address = 0;

  uintptr_t first_iteration_array[TEMP_MEMORY_SIZE];

  while (true)
  {

    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    uintptr_t x1[BIT_WIDTH];
    uintptr_t x2[BIT_WIDTH];
    uintptr_t x3[BIT_WIDTH];
    uintptr_t x4[BIT_WIDTH];
    uintptr_t x5[BIT_WIDTH];

    uintptr_t y1[BIT_WIDTH];
    uintptr_t y2[BIT_WIDTH];
    uintptr_t y3[BIT_WIDTH];
    uintptr_t y4[BIT_WIDTH];
    uintptr_t y5[BIT_WIDTH];

    uintptr_t zx1, zx2, zx3;
    uintptr_t zx4, zx5;

    uintptr_t nx1, nx2, nx3;
    uintptr_t nx4, nx5;

    uintptr_t zy1, zy2, zy3;
    uintptr_t zy4, zy5;

    uintptr_t ny1, ny2, ny3;
    uintptr_t ny4, ny5;

    uintptr_t f1, f2, f3;
    uintptr_t f4, f5;

    uintptr_t no1, no2, no3;
    uintptr_t no4, no5;

    uintptr_t out1[BIT_WIDTH];
    uintptr_t out2[BIT_WIDTH];
    uintptr_t out3[BIT_WIDTH];
    uintptr_t out4[BIT_WIDTH];
    uintptr_t out5[BIT_WIDTH];

    uintptr_t out_maj[BIT_WIDTH];

    uintptr_t read_output[BIT_WIDTH];

    uintptr_t x_arch[BIT_WIDTH];
    uintptr_t y_arch[BIT_WIDTH];

    uintptr_t zx_arch;
    uintptr_t nx_arch;
    uintptr_t zy_arch;
    uintptr_t ny_arch;

    uintptr_t f_arch;

    uintptr_t no_arch;

    uintptr_t expected_output[BIT_WIDTH];
    uintptr_t expected_zr;
    uintptr_t expected_ng;



    if (!good_accuracy_found)
    {
      randomise_temp_memory();
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
      {
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

      
      
      if (good_accuracy_found)
      {
        for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
        {
          temporary_memory[i] = first_iteration_array[i];
        }
      }
      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        x_arch[i] = rand() & 1;
        y_arch[i] = rand() & 1;
      }
      zx_arch = rand() & 1;
      nx_arch = rand() & 1;
      zy_arch = rand() & 1;
      ny_arch = rand() & 1;
      f_arch = rand() & 1;
      no_arch = rand() & 1;

      alu_simulate(x_arch, y_arch, zx_arch, nx_arch, zy_arch, ny_arch,
                   f_arch, no_arch, expected_output, &expected_zr, &expected_ng);

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        x1[i] = get_temporary_memory();
        x2[i] = get_temporary_memory();
        x3[i] = get_temporary_memory();

        x4[i] = get_temporary_memory();
        x5[i] = get_temporary_memory();

        y1[i] = get_temporary_memory();
        y2[i] = get_temporary_memory();
        y3[i] = get_temporary_memory();

        y4[i] = get_temporary_memory();
        y5[i] = get_temporary_memory();

        out1[i] = get_temporary_memory();
        out2[i] = get_temporary_memory();
        out3[i] = get_temporary_memory();

        out4[i] = get_temporary_memory();
        out5[i] = get_temporary_memory();

        out_maj[i] = get_temporary_memory();
      }
      zx1 = get_temporary_memory();
      zx2 = get_temporary_memory();
      zx3 = get_temporary_memory();

      zx4 = get_temporary_memory();
      zx5 = get_temporary_memory();

      nx1 = get_temporary_memory();
      nx2 = get_temporary_memory();
      nx3 = get_temporary_memory();

      nx4 = get_temporary_memory();
      nx5 = get_temporary_memory();

      zy1 = get_temporary_memory();
      zy2 = get_temporary_memory();
      zy3 = get_temporary_memory();

      zy4 = get_temporary_memory();
      zy5 = get_temporary_memory();

      ny1 = get_temporary_memory();
      ny2 = get_temporary_memory();
      ny3 = get_temporary_memory();

      ny4 = get_temporary_memory();
      ny5 = get_temporary_memory();

      f1 = get_temporary_memory();
      f2 = get_temporary_memory();
      f3 = get_temporary_memory();

      f4 = get_temporary_memory();
      f5 = get_temporary_memory();

      no1 = get_temporary_memory();
      no2 = get_temporary_memory();
      no3 = get_temporary_memory();

      no4 = get_temporary_memory();
      no5 = get_temporary_memory();

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        clflush(x1[i]);
        clflush(x2[i]);
        clflush(x3[i]);

        clflush(x4[i]);
        clflush(x5[i]);

        clflush(y1[i]);
        clflush(y2[i]);
        clflush(y3[i]);

        clflush(y4[i]);
        clflush(y5[i]);

        mfence();
        lfence();

        if (x_arch[i])
        {
          trash = FORCE_READ(x1[i], trash);
          trash = FORCE_READ(x2[i], trash);
          trash = FORCE_READ(x3[i], trash);

          trash = FORCE_READ(x4[i], trash);
          trash = FORCE_READ(x5[i], trash);
        }

        if (y_arch[i])
        {
          trash = FORCE_READ(y1[i], trash);
          trash = FORCE_READ(y2[i], trash);
          trash = FORCE_READ(y3[i], trash);

          trash = FORCE_READ(y4[i], trash);
          trash = FORCE_READ(y5[i], trash);
        }

        mfence();
        lfence();
      }
      clflush(nx1);
      clflush(nx2);
      clflush(nx3);

      clflush(nx4);
      clflush(nx5);

      clflush(zx1);
      clflush(zx2);
      clflush(zx3);

      clflush(zx4);
      clflush(zx5);

      clflush(zy1);
      clflush(zy2);
      clflush(zy3);

      clflush(zy4);
      clflush(zy5);

      clflush(ny1);
      clflush(ny2);
      clflush(ny3);

      clflush(ny4);
      clflush(ny5);

      clflush(f1);
      clflush(f2);
      clflush(f3);

      clflush(f4);
      clflush(f5);

      clflush(no1);
      clflush(no2);
      clflush(no3);

      clflush(no4);
      clflush(no5);

      mfence();
      lfence();

      if (zx_arch)
      {
        trash = FORCE_READ(zx1, trash);
        trash = FORCE_READ(zx2, trash);
        trash = FORCE_READ(zx3, trash);

        trash = FORCE_READ(zx4, trash);
        trash = FORCE_READ(zx5, trash);
      }

      if (nx_arch)
      {
        trash = FORCE_READ(nx1, trash);
        trash = FORCE_READ(nx2, trash);
        trash = FORCE_READ(nx3, trash);

        trash = FORCE_READ(nx4, trash);
        trash = FORCE_READ(nx5, trash);
      }

      if (zy_arch)
      {
        trash = FORCE_READ(zy1, trash);
        trash = FORCE_READ(zy2, trash);
        trash = FORCE_READ(zy3, trash);

        trash = FORCE_READ(zy4, trash);
        trash = FORCE_READ(zy5, trash);
      }

      if (ny_arch)
      {
        trash = FORCE_READ(ny1, trash);
        trash = FORCE_READ(ny2, trash);
        trash = FORCE_READ(ny3, trash);

        trash = FORCE_READ(ny4, trash);
        trash = FORCE_READ(ny5, trash);
      }

      if (f_arch)
      {
        trash = FORCE_READ(f1, trash);
        trash = FORCE_READ(f2, trash);
        trash = FORCE_READ(f3, trash);

        trash = FORCE_READ(f4, trash);
        trash = FORCE_READ(f5, trash);
      }

      if (no_arch)
      {
        trash = FORCE_READ(no1, trash);
        trash = FORCE_READ(no2, trash);
        trash = FORCE_READ(no3, trash);

        trash = FORCE_READ(no4, trash);
        trash = FORCE_READ(no5, trash);
      }

      mfence();
      lfence();

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        clflush(out1[i]);
        clflush(out2[i]);
        clflush(out3[i]);

        clflush(out4[i]);
        clflush(out5[i]);

        clflush(out_maj[i]);

        mfence();
        lfence();
      }
      
      #ifdef GATES_STATS
        gettimeofday(&start, NULL);
      #endif

      mfence();
      lfence();

      trash = alu_impl(x1, y1, zx1, nx1, zy1, ny1, f1, no1, out1, trash);

      mfence();
      lfence();

      trash = alu_impl(x2, y2, zx2, nx2, zy2, ny2, f2, no2, out2, trash);

      mfence();
      lfence();

      trash = alu_impl(x3, y3, zx3, nx3, zy3, ny3, f3, no3, out3, trash);

      mfence();
      lfence();

      trash = alu_impl(x4, y4, zx4, nx4, zy4, ny4, f4, no4, out4, trash);

      mfence();
      lfence();

      trash = alu_impl(x5, y5, zx5, nx5, zy5, ny5, f5, no5, out5, trash);

      mfence();
      lfence();
      

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        
        trash = majority_gate_5(out1[i], out2[i], out3[i], out4[i], out5[i], out_maj[i], trash);
      }
      
      #ifdef GATES_STATS
        gettimeofday(&stop, NULL);
        circuit_execution_time_us += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
      #endif
      

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        trash = read_addr(out_maj[i], &read_output[i], trash);
      }

      mfence();
      lfence();

      
      
      
      
      
      

      

      
      
      
      
      
      

      
      
      bool all_bits_correct = true;

      for (uintptr_t i = 0; i < BIT_WIDTH; i++)
      {
        if (read_output[i] != expected_output[i])
          all_bits_correct = false;
      }
      correct += all_bits_correct;
      incorrect += !all_bits_correct;

      return_temporary_memory(no5);
      return_temporary_memory(no4);
      return_temporary_memory(no3);
      return_temporary_memory(no2);
      return_temporary_memory(no1);

      return_temporary_memory(f5);
      return_temporary_memory(f4);
      return_temporary_memory(f3);
      return_temporary_memory(f2);
      return_temporary_memory(f1);

      return_temporary_memory(ny5);
      return_temporary_memory(ny4);
      return_temporary_memory(ny3);
      return_temporary_memory(ny2);
      return_temporary_memory(ny1);

      return_temporary_memory(zy5);
      return_temporary_memory(zy4);
      return_temporary_memory(zy3);
      return_temporary_memory(zy2);
      return_temporary_memory(zy1);

      return_temporary_memory(nx5);
      return_temporary_memory(nx4);
      return_temporary_memory(nx3);
      return_temporary_memory(nx2);
      return_temporary_memory(nx1);

      return_temporary_memory(zx5);
      return_temporary_memory(zx4);
      return_temporary_memory(zx3);
      return_temporary_memory(zx2);
      return_temporary_memory(zx1);

      for (intptr_t i = BIT_WIDTH - 1; i >= 0; i--)
      {
        return_temporary_memory(out_maj[i]);
        return_temporary_memory(out5[i]);
        return_temporary_memory(out4[i]);
        return_temporary_memory(out3[i]);
        return_temporary_memory(out2[i]);
        return_temporary_memory(out1[i]);

        return_temporary_memory(y5[i]);
        return_temporary_memory(y4[i]);
        return_temporary_memory(y3[i]);
        return_temporary_memory(y2[i]);
        return_temporary_memory(y1[i]);

        return_temporary_memory(x5[i]);
        return_temporary_memory(x4[i]);
        return_temporary_memory(x3[i]);
        return_temporary_memory(x2[i]);
        return_temporary_memory(x1[i]);
      }

    } 
    
    #ifdef GATES_STATS
      printf("Execution time averaged over %ld runs: %lf\n", iteration, circuit_execution_time_us / (double)iteration);
      printf("Gates count: %lf\n", gates_count / (double)iteration);
      printf("Intermediate values: %lf\n", intermediate_values / (double)iteration);
    #endif


    double TARGET_ACCURACY = 1.0;
    
    

    const double accuracy = (double)correct / (double)(incorrect + correct);

    if (good_accuracy_found || accuracy >= TARGET_ACCURACY)
      printf("ALU Accuracy : %d / %d (%lf)\n", correct, correct + incorrect,
             accuracy);
    if (accuracy < TARGET_ACCURACY && !good_accuracy_found)
    {
      printf("accuracy fell short, randomising temp memory!\n");
      target_accuracy_success = 0;
      failed_to_get_good_address++;
      if (failed_to_get_good_address >= FAIL_TO_FIND_THRESHOLD)
        exit(0);
      
    }
    else
    {
      if (!good_accuracy_found)
      {
        good_accuracy_found = true;

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