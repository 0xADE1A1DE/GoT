#include <stdlib.h>
#include <stdio.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

#define LIMIT_LOW(x) ((x < 0) ? GAME_OF_LIFE_DIMENSION - 1 : x);
#define LIMIT_HIGH(x) (x >= GAME_OF_LIFE_DIMENSION ? 0 : x);

uintptr_t gol_game(uintptr_t initial_state[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION],
                   uintptr_t iteration, uintptr_t trash)
{

  srand(time(NULL));
  bool correct_found = false;

  while (true)
  {

    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        initial_state[i][j] = ((rand() % 100) < 50);
      }
    }

    printf("Intial State\n");

    uintptr_t current_arch[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        if (initial_state[i][j])
          printf("#");
        else
          printf(".");
        current_arch[i][j] = initial_state[i][j];
      }
      printf("\n");
    }
    printf("\n");

    for (uintptr_t ctr = 0; ctr < iteration; ctr++)
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

    printf("Simulated Final State\n");

    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        if (current_arch[i][j])
          printf("#");
        else
          printf(".");
      }
      printf("\n");
    }
    printf("\n");

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

    for (uintptr_t ctr = 0; ctr < iteration; ctr++)
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

    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    printf("Microarchitecural State\n");
    uintptr_t read_mem;

    for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
    {
      for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
      {
        if (read_gol[i][j])
          printf("#");
        else
          printf(".");

        if (read_gol[i][j] == current_arch[i][j])
          correct++;
        else
          incorrect++;
      }
      printf("\n");
    }
    printf("\n");

    const double accuracy = (double)(correct) / (double)(correct + incorrect);
    printf("Accuracy = %ld / %ld (%lf)\n", correct, correct + incorrect,
           accuracy);

    for (intptr_t i = GAME_OF_LIFE_DIMENSION - 1; i >= 0; i--)
    {
      for (intptr_t j = GAME_OF_LIFE_DIMENSION - 1; j >= 0; j--)
      {
        return_temporary_memory(map[i][j]);
      }
    }

    if (accuracy >= 0.8)
    {
      correct_found = true;
      printf("High accuracy round found, not randomising!\n");
    }
    else
    {
      
      printf("Randomising temp memory!\n");
      randomise_temp_memory();
      
      
      
    }
    printf("===================================\n");
    printf("\n");
  }

  return trash;
}

#define TARGET_SUCCESS_COUNTS 50

uintptr_t gol_game_acccuracy_test(uintptr_t trash, uintptr_t iteration)
{
  uintptr_t game_iteration = 1;
  srand(time(NULL));
  bool correct_found = false;
  
  uintptr_t target_accuracy_success = 0;

  while (true)
  {
    uintptr_t correct = 0;
    uintptr_t incorrect = 0;

    for (uintptr_t i = 0; i < iteration; i++)
    {

      
      
      
      
      
      
      
      

      uintptr_t initial_state[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          initial_state[i][j] = ((rand() % 100) < 50);
        }
      }

      printf("Intial State\n");

      uintptr_t current_arch[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          if (initial_state[i][j])
            printf("#");
          else
            printf(".");
          current_arch[i][j] = initial_state[i][j];
        }
        printf("\n");
      }
      printf("\n");

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

      printf("Simulated Final State\n");

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          if (current_arch[i][j])
            printf("#");
          else
            printf(".");
        }
        printf("\n");
      }
      printf("\n");

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

      printf("Microarchitecural State\n");
      uintptr_t read_mem;

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          if (read_gol[i][j])
            printf("#");
          else
            printf(".");

          if (read_gol[i][j] == current_arch[i][j])
            correct++;
          else
            incorrect++;
        }
        printf("\n");
      }
      printf("\n");

      for (intptr_t i = GAME_OF_LIFE_DIMENSION - 1; i >= 0; i--)
      {
        for (intptr_t j = GAME_OF_LIFE_DIMENSION - 1; j >= 0; j--)
        {
          return_temporary_memory(map[i][j]);
        }
      }
    }
    
    const double accuracy = (double)(correct) / (double)(correct + incorrect);
    printf("Accuracy = %ld / %ld (%lf)\n", correct, correct + incorrect,
            accuracy);


    if (accuracy < TARGET_ACCURACY)
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











  










      



















































































































































































































































































    


























      















uintptr_t gol_game_acccuracy_test_majority(uintptr_t trash, uintptr_t iteration)
{
  uintptr_t game_iteration = 1;
  srand(time(NULL));
  bool correct_found = false;
  
  uintptr_t target_accuracy_success = 0;

  while (true)
  {
    uintptr_t correct = 0;
    uintptr_t incorrect = 0;
    
    uintptr_t maj_read = get_temporary_memory();

    for (uintptr_t i = 0; i < iteration; i++)
    {

      
      
      
      
      
      
      
      

      uintptr_t initial_state[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          initial_state[i][j] = ((rand() % 100) < 50);
        }
      }

      printf("Intial State\n");

      uintptr_t current_arch[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          if (initial_state[i][j])
            printf("#");
          else
            printf(".");
          current_arch[i][j] = initial_state[i][j];
        }
        printf("\n");
      }
      printf("\n");

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

      printf("Simulated Final State\n");

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          if (current_arch[i][j])
            printf("#");
          else
            printf(".");
        }
        printf("\n");
      }
      printf("\n");

      uintptr_t map1[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
      uintptr_t next_map1[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
      
      uintptr_t map2[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
      uintptr_t next_map2[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
      
      uintptr_t map3[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];
      uintptr_t next_map3[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          map1[i][j] = get_temporary_memory();
          map2[i][j] = get_temporary_memory();
          map3[i][j] = get_temporary_memory();
          
          mfence();
          lfence();

          clflush(map1[i][j]);
          clflush(map2[i][j]);
          clflush(map3[i][j]);

          mfence();
          lfence();

          if (initial_state[i][j]) {
            
            trash = FORCE_READ(map1[i][j], trash);
            trash = FORCE_READ(map2[i][j], trash);
            trash = FORCE_READ(map3[i][j], trash);
          }
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
            next_map1[i][j] = get_temporary_memory();
            next_map2[i][j] = get_temporary_memory();
            next_map3[i][j] = get_temporary_memory();
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

            uintptr_t neighbor_map_copy_1_1[9];
            uintptr_t neighbor_map_copy_2_1[9];
            
            uintptr_t neighbor_map_copy_1_2[9];
            uintptr_t neighbor_map_copy_2_2[9];
            
            uintptr_t neighbor_map_copy_1_3[9];
            uintptr_t neighbor_map_copy_2_3[9];

    
            for (uintptr_t i = 0; i < 9; i++)
            {
              neighbor_map_copy_1_1[i] = get_temporary_memory();
              neighbor_map_copy_2_1[i] = get_temporary_memory();
              
              neighbor_map_copy_1_2[i] = get_temporary_memory();
              neighbor_map_copy_2_2[i] = get_temporary_memory();
              
              neighbor_map_copy_1_3[i] = get_temporary_memory();
              neighbor_map_copy_2_3[i] = get_temporary_memory();

              mfence();
              lfence();
              clflush(neighbor_map_copy_1_1[i]);
              clflush(neighbor_map_copy_2_1[i]);
              
              clflush(neighbor_map_copy_1_2[i]);
              clflush(neighbor_map_copy_2_2[i]);
              
              clflush(neighbor_map_copy_1_3[i]);
              clflush(neighbor_map_copy_2_3[i]);
              mfence();
              lfence();
              trash = fan_gate(map1[neighbours[2 * i]][neighbours[2 * i + 1]],
                               neighbor_map_copy_1_1[i], neighbor_map_copy_2_1[i], trash);
              
              trash = fan_gate(map2[neighbours[2 * i]][neighbours[2 * i + 1]],
                               neighbor_map_copy_1_2[i], neighbor_map_copy_2_2[i], trash);
              
              trash = fan_gate(map3[neighbours[2 * i]][neighbours[2 * i + 1]],
                               neighbor_map_copy_1_3[i], neighbor_map_copy_2_3[i], trash);
              mfence();
              lfence();
            }

            clflush(next_map1[position_y][position_x]);
            mfence();
            lfence();

            trash = gol_gate_impl(neighbor_map_copy_1_1, next_map1[position_y][position_x], trash);

            clflush(next_map2[position_y][position_x]);
            mfence();
            lfence();
            
            trash = gol_gate_impl(neighbor_map_copy_1_2, next_map2[position_y][position_x], trash);
            
            clflush(next_map3[position_y][position_x]);
            mfence();
            lfence();
            
            trash = gol_gate_impl(neighbor_map_copy_1_3, next_map3[position_y][position_x], trash);
            
            mfence();
            lfence();

  
            for (uintptr_t i = 0; i < 9; i++)
            {
              
              
              clflush(map1[neighbours[i * 2]][neighbours[i * 2 + 1]]);
              mfence();
              lfence();
              trash = fan_gate(neighbor_map_copy_2_1[i], map1[neighbours[i * 2]][neighbours[i * 2 + 1]],
                               &trash, trash);
              
              clflush(map2[neighbours[i * 2]][neighbours[i * 2 + 1]]);
              mfence();
              lfence();
              trash = fan_gate(neighbor_map_copy_2_2[i], map2[neighbours[i * 2]][neighbours[i * 2 + 1]],
                               &trash, trash);
              
              clflush(map3[neighbours[i * 2]][neighbours[i * 2 + 1]]);
              mfence();
              lfence();
              trash = fan_gate(neighbor_map_copy_2_3[i], map3[neighbours[i * 2]][neighbours[i * 2 + 1]],
                               &trash, trash);
            }
            mfence();
            lfence();

            for (intptr_t i = 8; i >= 0; i--)
            {
              return_temporary_memory(neighbor_map_copy_2_3[i]);
              return_temporary_memory(neighbor_map_copy_2_2[i]);
              return_temporary_memory(neighbor_map_copy_2_1[i]);
              
              return_temporary_memory(neighbor_map_copy_1_3[i]);
              return_temporary_memory(neighbor_map_copy_1_2[i]);
              return_temporary_memory(neighbor_map_copy_1_1[i]);
            }
          }
        }

        for (intptr_t i = GAME_OF_LIFE_DIMENSION - 1; i >= 0; i--)
        {
          for (intptr_t j = GAME_OF_LIFE_DIMENSION - 1; j >= 0; j--)
          {
            return_temporary_memory(map3[i][j]);
            map3[i][j] = next_map3[i][j];
            
            return_temporary_memory(map2[i][j]);
            map2[i][j] = next_map2[i][j];
            
            return_temporary_memory(map1[i][j]);
            map1[i][j] = next_map1[i][j];
          }
        }
      }

      uintptr_t read_gol[GAME_OF_LIFE_DIMENSION][GAME_OF_LIFE_DIMENSION];

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          mfence();
          lfence();
          clflush(maj_read);
          mfence();
          lfence();
          
          trash = majority_gate(map1[i][j], map2[i][j], map3[i][j], maj_read,
                                trash);
          
          mfence();
          lfence();
          
          trash = read_addr(maj_read, &read_gol[i][j], trash);
          mfence();
          lfence();
        }
      }

#define TARGET_ACCURACY 0.9

      printf("Microarchitecural State\n");
      uintptr_t read_mem;

      for (uintptr_t i = 0; i < GAME_OF_LIFE_DIMENSION; i++)
      {
        for (uintptr_t j = 0; j < GAME_OF_LIFE_DIMENSION; j++)
        {
          if (read_gol[i][j])
            printf("#");
          else
            printf(".");

          if (read_gol[i][j] == current_arch[i][j])
            correct++;
          else
            incorrect++;
        }
        printf("\n");
      }
      printf("\n");

      for (intptr_t i = GAME_OF_LIFE_DIMENSION - 1; i >= 0; i--)
      {
        for (intptr_t j = GAME_OF_LIFE_DIMENSION - 1; j >= 0; j--)
        {          
          return_temporary_memory(map3[i][j]);
          return_temporary_memory(map2[i][j]);
          return_temporary_memory(map1[i][j]);
        }
      }
    }
    
    const double accuracy = (double)(correct) / (double)(correct + incorrect);
    printf("Accuracy = %ld / %ld (%lf)\n", correct, correct + incorrect,
            accuracy);


    if (accuracy < 0.8)
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
        break;
      }
      else
      {
        printf("Target accuracy reached, doing %ld more time(s) with the same configuration\n",
               TARGET_SUCCESS_COUNTS - target_accuracy_success);
      }
    }
    return_temporary_memory(maj_read);
  }

  return trash;
}