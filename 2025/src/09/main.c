#include "../../c_deps/grid.h"
#include "../../c_deps/utarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 100
#define BUFFER_SIZE 512

#define DAY_NUMBER "09"
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
