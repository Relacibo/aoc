#include "../../c_deps/grid.h"
#include "../../c_deps/utarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 100
#define BUFFER_SIZE 512

#define DAY_NUMBER "04"
#define INPUT_FILE_NAME "input"
// #define INPUT_FILE_NAME "example"

#ifndef PROJECT_ROOT_DIR
#define PROJECT_ROOT_DIR "."
#endif

#define DEBUG_PRINT_SOLUTION_1 0
#define DEBUG_PRINT_SOLUTION_2 1

Grid *read_input_file() {
  char input_file_path[MAX_PATH_LENGTH];

  snprintf(input_file_path, MAX_PATH_LENGTH, "%s/%s", PROJECT_ROOT_DIR,
           "resources/" DAY_NUMBER "/" INPUT_FILE_NAME);
  FILE *input_file;
  char buffer[BUFFER_SIZE];
  printf("Reading file: %s\n", input_file_path);

  input_file = fopen(input_file_path, "r");
  if (input_file == NULL) {
    perror("Error reading file");
    return NULL;
  }

  UT_array *grid_input;
  utarray_new(grid_input, &ut_ptr_icd);
  UT_array *current_row = NULL;
  size_t width = 0;
  size_t height = 0;
  while (fgets(buffer, BUFFER_SIZE, input_file) != NULL) {
    size_t len = strcspn(buffer, "\n");
    if (len == 0) {
      continue;
    }

    if (current_row == NULL) {
      utarray_new(current_row, &ut_int_icd);
    }

    for (size_t i = 0; i < len; i++) {
      int char_as_int = (int)buffer[i];
      utarray_push_back(current_row, &char_as_int);
    }
    utarray_push_back(grid_input, &current_row);
    if (len > width) {
      width = len;
    }
    height += 1;
    current_row = NULL;
  }

  if (current_row != NULL) {
    utarray_free(current_row);
  }
  fclose(input_file);

  Grid *grid = grid_new(grid_input, width, height);
  return grid;
}

unsigned long solution1(Grid *grid) {
  Direction *dirs = all_directions();
  UT_array **current_row_ptr = (UT_array **)utarray_front(grid->content);
  size_t y = 0;
  unsigned long sum = 0;
  while (current_row_ptr != NULL) {
    size_t x = 0;
    UT_array *current_row = *current_row_ptr;
    int *char_val = (int *)utarray_front(current_row);
    while (char_val != NULL) {
      if (*char_val == '@') {
        Coords coords;
        coords = (Coords){
            .x = x,
            .y = y,
        };
        unsigned int rolls_count = 0;
        for (size_t i = 0; i < ALL_DIRECTIONS_LEN; i++) {
          Direction direction = dirs[i];
          Coords new_coords;
          char val;
          if (grid_move_direction(&new_coords, &val, grid, &coords, direction,
                                  1) == MOVE_SUCCESS) {
            if (val == '@' || val == 'x') {
              rolls_count += 1;
            }
          }
        }
        if (rolls_count < 4) {
          sum += 1;
          if (DEBUG_PRINT_SOLUTION_1) {
            *char_val = (int)'x';
          }
        }
      }
      char_val = (int *)utarray_next(current_row, char_val);
      x++;
    }
    current_row_ptr = (UT_array **)utarray_next(grid->content, current_row_ptr);
    y++;
  }
  free(dirs);
  return sum;
}

unsigned long solution2(Grid *grid) {
  Direction *dirs = all_directions();
  unsigned long sum = 0;
  unsigned int step = 1;
  unsigned long removed_last_round = 1;
  while (removed_last_round > 0) {
    removed_last_round = 0;
    size_t y = 0;
    UT_array **current_row_ptr = (UT_array **)utarray_front(grid->content);
    while (current_row_ptr != NULL) {
      size_t x = 0;
      UT_array *current_row = *current_row_ptr;
      int *char_val = (int *)utarray_front(current_row);
      while (char_val != NULL) {
        if (*char_val == '@') {
          Coords coords;
          coords = (Coords){
              .x = x,
              .y = y,
          };
          unsigned int rolls_count = 0;
          for (size_t i = 0; i < ALL_DIRECTIONS_LEN; i++) {
            Direction direction = dirs[i];
            Coords new_coords;
            char val;
            if (grid_move_direction(&new_coords, &val, grid, &coords, direction,
                                    1) == MOVE_SUCCESS) {
              if (val == '@' || val == 'x') {
                rolls_count += 1;
              }
            }
          }
          if (rolls_count < 4) {
            sum += 1;
            removed_last_round += 1;
            *char_val = (int)'x';
          }
        }
        char_val = (int *)utarray_next(current_row, char_val);
        x++;
      }
      current_row_ptr =
          (UT_array **)utarray_next(grid->content, current_row_ptr);
      y++;
    }
    if (DEBUG_PRINT_SOLUTION_2) {
      printf("\n");
      printf("Step %d:\n", step);
      grid_print(grid);
    }
    grid_replace_all(grid, 'x', '.');
    step++;
  }
  free(dirs);
  return sum;
}

int main() {
  Grid *grid = read_input_file();
  if (grid == NULL) {
    return 1;
  }
  unsigned long res1 = solution1(grid);
  printf("Solution 1: %lu\n", res1);

  if (DEBUG_PRINT_SOLUTION_1) {
    grid_print(grid);
  }
  grid_free(grid);

  grid = read_input_file();
  if (DEBUG_PRINT_SOLUTION_2) {
    printf("\n");
    printf("Initial configuration:\n");
    grid_print(grid);
  }
  unsigned long res2 = solution2(grid);
  printf("Solution 2: %lu\n", res2);

  grid_free(grid);
  return 0;
}
