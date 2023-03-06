#include <stdlib.h>
#include <stdio.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t gol_gate_impl(uintptr_t neighbours[9], 
                        uintptr_t next_state, uintptr_t trash) {
  uintptr_t neighbour_0 = neighbours[0];
  uintptr_t neighbour_1 = neighbours[1];
  uintptr_t neighbour_2 = neighbours[2];
  uintptr_t neighbour_3 = neighbours[3];
  uintptr_t neighbour_4 = neighbours[4];
  uintptr_t neighbour_5 = neighbours[5];
  uintptr_t neighbour_6 = neighbours[6];
  uintptr_t neighbour_7 = neighbours[7];
  
  uintptr_t current_state = neighbours[8];
  
  uintptr_t ha1_sum = get_temporary_memory();
  uintptr_t ha1_carry = get_temporary_memory();
  
  uintptr_t ha2_sum = get_temporary_memory();
  uintptr_t ha2_carry = get_temporary_memory();
  
  uintptr_t ha3_sum = get_temporary_memory();
  uintptr_t ha3_carry = get_temporary_memory();
  
  uintptr_t ha4_sum = get_temporary_memory();
  uintptr_t ha4_carry = get_temporary_memory();
  
  uintptr_t ha5_sum = get_temporary_memory();
  uintptr_t ha5_carry = get_temporary_memory();
  
  uintptr_t ha6_sum = get_temporary_memory();
  uintptr_t ha6_carry = get_temporary_memory();

  uintptr_t ha7_sum = get_temporary_memory();
  uintptr_t ha7_carry = get_temporary_memory();
  
  uintptr_t ha8_sum = get_temporary_memory();
  uintptr_t ha8_carry = get_temporary_memory();
  
  uintptr_t ha9_sum = get_temporary_memory();
  uintptr_t ha9_carry = get_temporary_memory();
  
  uintptr_t ha10_sum = get_temporary_memory();
  uintptr_t ha10_carry = get_temporary_memory();
  
  uintptr_t or1_out = get_temporary_memory();
  uintptr_t or2_out = get_temporary_memory();
  uintptr_t or3_out = get_temporary_memory();
  uintptr_t or4_out = get_temporary_memory();
  uintptr_t or5_out = get_temporary_memory();

  uintptr_t nor_out = get_temporary_memory();
  uintptr_t and_out = get_temporary_memory();
  
  mfence();
  lfence();
  
  clflush(ha1_sum);
  clflush(ha1_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(neighbour_0, neighbour_1, ha1_sum, 
                          ha1_carry, trash);

  clflush(ha2_sum);
  clflush(ha2_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(neighbour_2, neighbour_3, ha2_sum, 
                          ha2_carry, trash);
  
  clflush(ha3_sum);
  clflush(ha3_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(neighbour_4, neighbour_5, ha3_sum, 
                          ha3_carry, trash);
  
  clflush(ha4_sum);
  clflush(ha4_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(neighbour_6, neighbour_7, ha4_sum, 
                          ha4_carry, trash);
  
  clflush(ha5_sum);
  clflush(ha5_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(ha1_sum, ha2_sum, ha5_sum, 
                          ha5_carry, trash);
  
  clflush(ha6_sum);
  clflush(ha6_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(ha3_sum, ha4_sum, ha6_sum, 
                          ha6_carry, trash);
  
  clflush(or1_out);
  
  mfence();
  lfence();
  
  trash = or_from_nor_not(ha5_carry, ha1_carry, or1_out, trash);
  
  clflush(or2_out);
  mfence();
  lfence();
  
  trash = or_from_nor_not(ha6_carry, ha3_carry, or2_out, trash);
  
  clflush(ha7_sum);
  clflush(ha7_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(or2_out, ha4_carry, ha7_sum, ha7_carry, trash);
  
  clflush(ha8_sum);
  clflush(ha8_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(ha5_sum, ha6_sum, ha8_sum, ha8_carry, trash);
  
  clflush(or3_out);
  
  mfence();
  lfence();
  
  trash = or_from_nor_not(or1_out, ha8_carry, or3_out, trash);
  
  clflush(ha9_sum);
  clflush(ha9_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(or3_out, ha2_carry, ha9_sum, ha9_carry, trash);
  
  clflush(or4_out);
  
  mfence();
  lfence();
  
  trash = or_from_nor_not(ha9_carry, ha7_carry, or4_out, trash);
  
  clflush(ha10_sum);
  clflush(ha10_carry);
  
  mfence();
  lfence();
  
  trash = half_adder_impl(ha9_sum, ha7_sum, ha10_sum, ha10_carry, trash);
  
  clflush(nor_out);
  
  mfence();
  lfence();
  
  trash = nor_fan_gate(ha10_carry, or4_out, nor_out, trash);
  
  clflush(or5_out);
  
  mfence();
  lfence();
  
  trash = or_from_nor_not(current_state, ha8_sum, or5_out, trash);
  
  clflush(and_out);
  
  mfence();
  lfence();
  
  trash = and_fan_gate(ha10_sum, or5_out, and_out, trash);
  
  clflush(next_state);
  
  mfence();
  lfence();
  
  trash = and_fan_gate(nor_out, and_out, next_state, trash);
  
  return_temporary_memory(and_out);
  return_temporary_memory(nor_out);
  return_temporary_memory(or5_out);
  return_temporary_memory(or4_out);
  return_temporary_memory(or3_out);
  return_temporary_memory(or2_out);
  return_temporary_memory(or1_out);
  
  return_temporary_memory(ha10_carry);
  return_temporary_memory(ha10_sum);
  
  return_temporary_memory(ha9_carry);
  return_temporary_memory(ha9_sum);
  
  return_temporary_memory(ha8_carry);
  return_temporary_memory(ha8_sum);
  
  return_temporary_memory(ha7_carry);
  return_temporary_memory(ha7_sum);
  
  return_temporary_memory(ha6_carry);
  return_temporary_memory(ha6_sum);
  
  return_temporary_memory(ha5_carry);
  return_temporary_memory(ha5_sum);
  
  return_temporary_memory(ha4_carry);
  return_temporary_memory(ha4_sum);
  
  return_temporary_memory(ha3_carry);
  return_temporary_memory(ha3_sum);
  
  return_temporary_memory(ha2_carry);
  return_temporary_memory(ha2_sum);
  
  return_temporary_memory(ha1_carry);
  return_temporary_memory(ha1_sum);
  
  
  
  
  
  
  
  
  
  
  return trash;
}

uintptr_t test_gol_gate_rec(uintptr_t state[8], uintptr_t counter, bool current, uintptr_t trash) {
  if (counter == 8) {
    uintptr_t input[9];
    for (uintptr_t i = 0; i < 9; i++) {
      input[i] = get_temporary_memory();
    }
    uintptr_t current_state = input[8];
    
    uintptr_t output = get_temporary_memory();
    uintptr_t output_read;
    
    printf("current : %d - ", current);
    
    for (uintptr_t i = 0; i < 8; i++) {
      printf("%ld ", state[i]);
    }
    printf(": ");
    
    for (uintptr_t i = 0; i < 8; i++) {
      clflush(input[i]);
      mfence();
      lfence();
      if (state[i])
        trash = FORCE_READ(input[i], trash);
      mfence();
      lfence();
    }
    
    mfence();
    lfence();
    
    clflush(current_state);
    clflush(output);
    
    mfence();
    lfence();
    
    if (current) {
      trash = FORCE_READ(current_state, trash);
    }
    mfence();
    lfence();
    
    trash = gol_gate_impl(input, output, trash);
    
    mfence();
    lfence();
    
    trash = read_addr(output, &output_read, trash);
    printf("%ld\n", output_read);
    
    return_temporary_memory(output);
    for (intptr_t i = 8; i >= 0; i--) {
      return_temporary_memory(input[i]);
    }
    
    return trash;
  } else {
    state[counter] = 0;
    trash += test_gol_gate_rec(state, counter + 1, current, trash);
    state[counter] = 1;
    trash += test_gol_gate_rec(state, counter + 1, current, trash);
    
    return trash;
  }
}

uintptr_t test_gol(uintptr_t trash) {
  uintptr_t state[8] = {0};
  
  trash = test_gol_gate_rec(state, 0, false, trash);
  return test_gol_gate_rec(state, 0, true, trash);
}






uintptr_t test_gol_gate_rec_accuracy(uintptr_t state[8], uintptr_t counter, bool current, uintptr_t trash,
                            uintptr_t* correct, uintptr_t* incorrect) {
  if (counter == 8) {
    uintptr_t input[9];
    for (uintptr_t i = 0; i < 9; i++) {
      input[i] = get_temporary_memory();
    }
    uintptr_t current_state = input[8];
    
    uintptr_t output = get_temporary_memory();
    uintptr_t output_read;
    
    
    
    uintptr_t alive_neighbours = 0;
    
    for (uintptr_t i = 0; i < 8; i++) {
      
      alive_neighbours += state[i];
    }
    
    
    for (uintptr_t i = 0; i < 8; i++) {
      clflush(input[i]);
      mfence();
      lfence();
      if (state[i])
        trash = FORCE_READ(input[i], trash);
      mfence();
      lfence();
    }
    
    mfence();
    lfence();
    
    clflush(current_state);
    clflush(output);
    
    mfence();
    lfence();
    
    if (current) {
      trash = FORCE_READ(current_state, trash);
    }
    mfence();
    lfence();
    
    trash = gol_gate_impl(input, output, trash);
    
    mfence();
    lfence();
    
    trash = read_addr(output, &output_read, trash);
    
    if (output_read && current && alive_neighbours >= 2 && alive_neighbours <= 3) {
      *correct = *correct + 1;
      
      
    } else if (output_read && !current && alive_neighbours == 3) {
      *correct = *correct + 1;
      
      
    } else if (!output_read && current && (alive_neighbours <= 1 || alive_neighbours >= 4)) {
      *correct = *correct + 1;
      
      
    } else if(!output_read && !current && alive_neighbours != 3) {
      *correct = *correct + 1;
      
      
    } else {
      *incorrect = *incorrect + 1;
      
      
      
      
      
      
      
    }

    return_temporary_memory(output);
    for (intptr_t i = 8; i >= 0; i--) {
      return_temporary_memory(input[i]);
    }
    
    return trash;
  } else {
    state[counter] = 0;
    trash += test_gol_gate_rec_accuracy(state, counter + 1, current, trash,
                                        correct, incorrect);
    state[counter] = 1;
    trash += test_gol_gate_rec_accuracy(state, counter + 1, current, trash, correct, incorrect);
    
    return trash;
  }
}
                                   
#define TARGET_SUCCESS_COUNTS 100

uintptr_t test_gol_accuracy(uintptr_t trash, uintptr_t iteration) {
  srand(time(NULL));
  uintptr_t target_accuracy_success = 0;
  
  while (true) {
  
  uintptr_t state[8] = {0};
  
  uintptr_t correct = 0;
  uintptr_t incorrect = 0;
  
  
  for (uintptr_t i = 0; i < iteration; i++) {
  
  trash = test_gol_gate_rec_accuracy(state, 0, false, trash,
                                     &correct, &incorrect);
  trash = test_gol_gate_rec_accuracy(state, 0, true, trash,
                                    &correct, &incorrect);
  
  
  }
  const double accuracy = (double)correct / (double)(incorrect + correct);
  
  printf("Game of Life Gate Accuracy: %ld / %ld (%lf)\n", correct, correct + incorrect,
         accuracy);
  
  
    if (accuracy < 0.9)
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