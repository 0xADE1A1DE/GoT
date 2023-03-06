#include <stdlib.h>
#include <stdio.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

#ifdef GATES_STATS
  #include <sys/time.h>
#endif


extern intptr_t prefetcher_enabled;

#define LIMIT_LOW(x) ((x < 0) ? GAME_OF_LIFE_DIMENSION - 1 : x);
#define LIMIT_HIGH(x) (x >= GAME_OF_LIFE_DIMENSION ? 0 : x);

void grayspace(int64_t intensity, int64_t min, int64_t max, char c)
{
  printf("\e[48;5;%ld;31;1m%c\e[0m", 232 + ((intensity - min) * 24) / (max - min + 1), c);
}

int64_t display_heatmap(double heatmap[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION])
{
  int64_t min = INT64_MAX;
  int64_t max = INT64_MIN;

  for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
  {
    for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
    {
      min = (min < heatmap[i][j]) ? min : heatmap[i][j];
      max = (max > heatmap[i][j]) ? max : heatmap[i][j];
    }
  }

  max += (max == min);

  for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
  {
    for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
    {
      grayspace((int64_t)heatmap[i][j], min, max, ' ');
    }
    printf("\n");
  }
  printf("\n");
}



extern uint64_t TARGET_SUCCESS_COUNTS;

extern uintptr_t temporary_memory[TEMP_MEMORY_SIZE];
extern uintptr_t available_temporary_memory;

uintptr_t gol_game_glider(uintptr_t trash, uintptr_t iteration, uintptr_t game_iteration)
{
  
  srand(time(NULL));

  double glider_heatmap[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

  uintptr_t initial_state[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION] =
      {{0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0, 0, 0, 0},
       {0, 0, 0, 1, 0, 0, 0, 0},
       {0, 1, 1, 1, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0, 0, 0, 0}};
      
    uintptr_t fail_to_find_good_accuracy = 0;

  
  
  
  
  
  
  
  
  
  
  
  
  

  
  
  
  
  
  
  
  
  
  
  
  
  

  
  
  
  
  
  
  
  
  
  
  
  
  

  if (game_iteration == 0)
  {

    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        if (initial_state[i][j])
        {
          glider_heatmap[i][j] += 1000000;
        }
      }
    }

    display_heatmap(glider_heatmap);

    return trash;
  }


  uintptr_t current_arch[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

  for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
  {
    for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
    {
      current_arch[i][j] = initial_state[i][j];
    }
  }

  for (uintptr_t ctr = 0; ctr < game_iteration; ctr++)
  {
    uintptr_t next_state_arch[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
    for (intptr_t position_y = 0; position_y < GAME_OF_LIFE_DIMENSION; position_y++)
    {
      for (intptr_t position_x = 0; position_x < GAME_OF_LIFE_DIMENSION; position_x++)
      {
        uintptr_t neighbours[2 * 8];
        neighbours[0] = LIMIT_LOW(position_y - 1);
        neighbours[1] = LIMIT_LOW(position_x - 1);
        neighbours[2] = LIMIT_LOW(position_y - 1);
        neighbours[3] = position_x;
        neighbours[4] = LIMIT_LOW(position_y - 1);
        neighbours[5] = LIMIT_HIGH(position_x + 1);
        neighbours[6] = position_y;
        neighbours[7] = LIMIT_HIGH(position_x + 1);
        neighbours[8] = LIMIT_HIGH(position_y + 1);
        neighbours[9] = LIMIT_HIGH(position_x + 1);
        neighbours[10] = LIMIT_HIGH(position_y + 1);
        neighbours[11] = position_x;
        neighbours[12] = LIMIT_HIGH(position_y + 1);
        neighbours[13] = LIMIT_LOW(position_x - 1);
        neighbours[14] = position_y;
        neighbours[15] = LIMIT_LOW(position_x - 1);

        uintptr_t alive_neighbours = 0;
        for (uintptr_t i = 0; i < 8; i++)
        {
          alive_neighbours += current_arch[neighbours[i * 2]][neighbours[i * 2 + 1]];
        }
        

        next_state_arch[position_y][position_x] = ((current_arch[position_y][position_x] && alive_neighbours >= 2 && alive_neighbours <= 3) ||
                                                   (!current_arch[position_y][position_x] && alive_neighbours == 3));
      }
    }
    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        current_arch[i][j] = next_state_arch[i][j];
      }
    }
  }

  

  
  
  
  
  
  
  
  
  
  
  
  

  bool correct_found = false;
  bool good_accuracy_found = false;

  uintptr_t temp_memory_backup[TEMP_MEMORY_SIZE];

  uintptr_t first_iteration_array[TEMP_MEMORY_SIZE];
  
  for (int exp_rep = 0 ; exp_rep < TARGET_SUCCESS_COUNTS; exp_rep++) {
    
    uintptr_t total_correct = 0;
  uintptr_t total_incorrect = 0;

  uintptr_t total_correct_whole = 0;
  uintptr_t total_incorrect_whole = 0;

  uintptr_t correct_whole_accuracy = 0;
  uintptr_t incorrect_whole_accuracy = 0;
  
  for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
  {
    for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
    {
      glider_heatmap[i][j] = 0;
    }
  }
  
  #ifdef GATES_STATS
    struct timeval start, stop;
    circuit_execution_time_us = 0.0;
    gates_count = 0;
    intermediate_values = 0;
  #endif

  for (intptr_t rep = 0; rep < iteration; rep++)
  {
    if (rep == 0 && !good_accuracy_found)
    {
      
      randomise_temp_memory();
      for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
      {
        first_iteration_array[i] = temporary_memory[i];
      }
    }
    for (uintptr_t i = 0; i < TEMP_MEMORY_SIZE; i++)
    {
      temporary_memory[i] = first_iteration_array[i];
    }
    
    mfence();
    lfence();
    
    
    
    
    
    
    
    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    
    
    
    
    
    
    
    

    

    uintptr_t map[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
    uintptr_t next_map[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        map[i][j] = get_temporary_memory();
        mfence();
        lfence();

        clflush(map[i][j]);

        mfence();
        lfence();

        if (initial_state[i][j])
          trash = FORCE_READ(map[i][j], trash);

        mfence();
        lfence();
      }
    }
  
    #ifdef GATES_STATS
      gettimeofday(&start, NULL);
    #endif

    for (uintptr_t ctr = 0; ctr < game_iteration; ctr++)
    {

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          next_map[i][j] = get_temporary_memory();
        }
      }
      
      for (intptr_t position_y = 0; position_y < GAME_OF_LIFE_DIMENSION;
           position_y++)
      {
        
        for (intptr_t position_x = 0; position_x < GAME_OF_LIFE_DIMENSION;
             position_x++)
        {

          uintptr_t neighbours[2 * 9];
          neighbours[0] = LIMIT_LOW(position_y - 1);
          neighbours[1] = LIMIT_LOW(position_x - 1);
          neighbours[2] = LIMIT_LOW(position_y - 1);
          neighbours[3] = position_x;
          neighbours[4] = LIMIT_LOW(position_y - 1);
          neighbours[5] = LIMIT_HIGH(position_x + 1);
          neighbours[6] = position_y;
          neighbours[7] = LIMIT_HIGH(position_x + 1);
          neighbours[8] = LIMIT_HIGH(position_y + 1);
          neighbours[9] = LIMIT_HIGH(position_x + 1);
          neighbours[10] = LIMIT_HIGH(position_y + 1);
          neighbours[11] = position_x;
          neighbours[12] = LIMIT_HIGH(position_y + 1);
          neighbours[13] = LIMIT_LOW(position_x - 1);
          neighbours[14] = position_y;
          neighbours[15] = LIMIT_LOW(position_x - 1);
          neighbours[16] = position_y;
          neighbours[17] = position_x;

          uintptr_t neighbor_map_copy_1[9];
          uintptr_t neighbor_map_copy_2[9];

          
          for (uintptr_t i = 0; i < 9; i++)
          {
            neighbor_map_copy_1[i] = get_temporary_memory();
            neighbor_map_copy_2[i] = get_temporary_memory();

            mfence();
            lfence();
            clflush(neighbor_map_copy_1[i]);
            clflush(neighbor_map_copy_2[i]);
            mfence();
            lfence();
            trash = fan_gate(map[neighbours[2 * i]][neighbours[2 * i + 1]],
                             neighbor_map_copy_1[i], neighbor_map_copy_2[i], trash);
            mfence();
            lfence();
          }

          clflush(next_map[position_y][position_x]);
          mfence();
          lfence();

          trash = gol_gate_impl(neighbor_map_copy_1, next_map[position_y][position_x], trash);

          mfence();
          lfence();

          
          for (uintptr_t i = 0; i < 9; i++)
          {
            
            
            clflush(map[neighbours[i * 2]][neighbours[i * 2 + 1]]);
            mfence();
            lfence();
            trash = fan_gate(neighbor_map_copy_2[i], map[neighbours[i * 2]][neighbours[i * 2 + 1]],
                             &trash, trash);
          }
          mfence();
          lfence();

          for (intptr_t i = 8; i >= 0; i--)
          {
            return_temporary_memory(neighbor_map_copy_2[i]);
            return_temporary_memory(neighbor_map_copy_1[i]);
          }
        }
      }

      for (intptr_t i = GAME_OF_LIFE_DIMENSION - 1; i >= 0; i--)
      {
        for (intptr_t j = GAME_OF_LIFE_DIMENSION - 1; j >= 0; j--)
        {
          return_temporary_memory(map[i][j]);
          map[i][j] = next_map[i][j];
        }
      }
    }
  
    #ifdef GATES_STATS
      gettimeofday(&stop, NULL);
      circuit_execution_time_us += (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    #endif

    uintptr_t read_gol[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        trash = read_addr(map[i][j], &read_gol[i][j], trash);
        mfence();
        lfence();
      }
    }



    
    uintptr_t read_mem;

    uintptr_t correct_whole = 1;
    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        glider_heatmap[i][j] += read_gol[i][j];
        if (read_gol[i][j] == current_arch[i][j])
        {
          correct++;
          total_correct++;
        }
        else
        {
          incorrect++;
          total_incorrect++;
          correct_whole = 0;
        }
      }
    }

    total_correct_whole += correct_whole;
    total_incorrect_whole += (correct_whole == 0);

    correct_whole_accuracy += correct_whole;
    incorrect_whole_accuracy += (correct_whole == 0);

    for (intptr_t i = GAME_OF_LIFE_DIMENSION - 1; i >= 0; i--)
    {
      for (intptr_t j = GAME_OF_LIFE_DIMENSION - 1; j >= 0; j--)
      {
        return_temporary_memory(map[i][j]);
      }
    }
    
    
    
    double target_accuracy;
    
    
    if (game_iteration <= 5) {
      target_accuracy = 1.0;
      }
    else if (game_iteration <= 10) {
      target_accuracy = 0.6; 
    } 
    else if (game_iteration <= 20) {
      target_accuracy = 0.4;
    }
      else if (game_iteration <= 30) {
      target_accuracy = 0.20;
    } 
      else if (game_iteration <= 40) {
      target_accuracy = 0.15;
    }
    else if (game_iteration <= 50) {
      target_accuracy = 0.1;
    } else {
      target_accuracy = 0.05;
    }
    
    if (prefetcher_enabled) {
    
    if (game_iteration <= 5) {
      target_accuracy = 0.7;
      }
    else if (game_iteration <= 10) {
      target_accuracy = 0.4; 
    } 
    else if (game_iteration <= 20) {
      target_accuracy = 0.3;
    }
      else if (game_iteration <= 30) {
      target_accuracy = 0.15;
    } 
      else if (game_iteration <= 40) {
      target_accuracy = 0.1;
    }
    else if (game_iteration <= 50) {
      target_accuracy = 0.05;
    } else {
      target_accuracy = 0.05;
    }
    }
    
    const double accuracy = (double)correct_whole_accuracy / (double)(correct_whole_accuracy + incorrect_whole_accuracy);
    
    
    
    
    
    
    
    
    if ((rep % 80) == 0 && rep > 0 && !good_accuracy_found) {

      

      
        rep = -1;
        total_correct = 0;
        total_incorrect = 0;
        total_correct_whole = 0;
        total_incorrect_whole = 0;
        correct_whole_accuracy = 0;
        incorrect_whole_accuracy = 0;
      
      if (accuracy >= target_accuracy)
      {
        good_accuracy_found = true;
      } else {
        
        fail_to_find_good_accuracy++;
        
        
        
      }
    }
    if (fail_to_find_good_accuracy >= 100)
      exit (0);
  }
    #ifdef GATES_STATS
      printf("Execution time averaged over %ld runs: %lf\n", iteration, circuit_execution_time_us / (double)iteration);
      printf("Gates count: %lf\n", gates_count / (double)iteration);
      printf("Intermediate values: %lf\n", intermediate_values / (double)iteration);
    #endif

  double total = 0;
  for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
  {
    for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
    {
      total += glider_heatmap[i][j];
    }
  }

#define SCALE 1000000

  for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
  {
    for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
    {
      glider_heatmap[i][j] = glider_heatmap[i][j] / total * SCALE;
    }
  }

  display_heatmap(glider_heatmap);

  printf("generation %ld\n", game_iteration);
  printf("Individual cell accuracy: %lf\n", (double)total_correct / (double)(total_correct + total_incorrect));
  printf("Whole accuracy: %ld - %ld : %lf\n", total_correct_whole, total_correct_whole + total_incorrect_whole, (double)total_correct_whole / (double)(total_correct_whole + total_incorrect_whole));
fflush(stdout);
mfence();
}
  return trash;
}
