#include <stdlib.h>
#include <stdio.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

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

uintptr_t calculate_next(uintptr_t position_y,
                         uintptr_t position_x,
                         uintptr_t map[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION],
                         uintptr_t next_map[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION],
                         uintptr_t neighbours[9],
                         uintptr_t trash)
{
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
  return trash;
}

uintptr_t gol_game_glider(uintptr_t trash, uintptr_t iteration, uintptr_t game_iteration)
{
  
  srand(time(NULL));
  bool correct_found = false;

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
  for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
  {
    for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
    {
      glider_heatmap[i][j] = 0;
    }
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

  

  
  
  
  
  
  
  
  
  
  
  
  

  uintptr_t total_correct = 0;
  uintptr_t total_incorrect = 0;

  uintptr_t total_correct_whole = 0;
  uintptr_t total_incorrect_whole = 0;

  uintptr_t correct_whole_accuracy = 0;
  uintptr_t incorrect_whole_accuracy = 0;

  for (uintptr_t i = 0; i < iteration; i++)
  {
    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    
    
    
    
    
    
    
    

    

    uintptr_t map[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

    uintptr_t map_copies_1[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
    uintptr_t map_copies_2[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
    uintptr_t map_copies_3[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
    
    uintptr_t next_map_copies_1[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
    uintptr_t next_map_copies_2[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
    uintptr_t next_map_copies_3[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

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

    for (uintptr_t ctr = 0; ctr < game_iteration; ctr++)
    {

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          next_map[i][j] = get_temporary_memory();
          map_copies_1[i][j] = get_temporary_memory();
          map_copies_2[i][j] = get_temporary_memory();
          map_copies_3[i][j] = get_temporary_memory();
          next_map_copies_1[i][j] = get_temporary_memory();
          next_map_copies_2[i][j] = get_temporary_memory();
          next_map_copies_3[i][j] = get_temporary_memory();
        }
      }
      
      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++) {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++) {
          clflush(map_copies_1[i][j]);
          clflush(map_copies_2[i][j]);
          clflush(map_copies_3[i][j]);
          mfence();
          lfence();
          trash = fan_gate_3(map[i][j], map_copies_1[i][j], map_copies_2[i][j],
                             map_copies_3[i][j], trash);
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
          
          clflush(next_map_copies_1[position_y][position_x]);
          mfence();
          lfence();
          trash = calculate_next(position_y, position_x, map_copies_1, next_map_copies_1,
                                 neighbours, trash);
          clflush(next_map_copies_2[position_y][position_x]);
          mfence();
          lfence();
          
          trash = calculate_next(position_y, position_x, map_copies_2, next_map_copies_2,
                                 neighbours, trash);
          clflush(next_map_copies_3[position_y][position_x]);
          mfence();
          lfence();
          trash = calculate_next(position_y, position_x, map_copies_3, next_map_copies_3,
                                 neighbours, trash);

        }
      }
      



      for (intptr_t i = GAME_OF_LIFE_DIMENSION - 1; i >= 0; i--)
      {
        for (intptr_t j = GAME_OF_LIFE_DIMENSION - 1; j >= 0; j--)
        { 
          clflush(next_map[i][j]);
          mfence();
          lfence();
          
          trash = majority_gate(next_map_copies_1[i][j], next_map_copies_2[i][j],
                                next_map_copies_3[i][j], next_map[i][j], trash);
          mfence();
          lfence();
          return_temporary_memory(next_map_copies_3[i][j]);
          return_temporary_memory(next_map_copies_2[i][j]);
          return_temporary_memory(next_map_copies_1[i][j]);
          
          return_temporary_memory(map_copies_3[i][j]);
          return_temporary_memory(map_copies_2[i][j]);
          return_temporary_memory(map_copies_1[i][j]);
          
          return_temporary_memory(map[i][j]);
          
          map[i][j] = next_map[i][j];
        }
      }
    }

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

#define TARGET_ACCURACY 0.9

    
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

    if ((i % 20) == 0 && i > 0)
    {
      const double accuracy = (double)correct_whole_accuracy / (double)(correct_whole_accuracy + incorrect_whole_accuracy);
      if (accuracy < 0.6)
      {
        randomise_temp_memory();
        correct_whole_accuracy = 0;
        incorrect_whole_accuracy = 0;
      }
    }
  }

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
  printf("Whole accuracy: %lf\n", (double)total_correct_whole / (double)(total_correct_whole + total_incorrect_whole));

  return trash;
}
