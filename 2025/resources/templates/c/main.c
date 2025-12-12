#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "../../c_deps/utarray.h"

#define MAX_PATH_LENGTH  100
#define BUFFER_SIZE  512

#define DAY_NUMBER "00"
#define INPUT_FILE_NAME "input"
// #define INPUT_FILE_NAME "example"

#ifndef PROJECT_ROOT_DIR
#define PROJECT_ROOT_DIR "."
#endif

#define DEBUG_PRINT_SOLUTION_1 0
#define DEBUG_PRINT_SOLUTION_2 1

typedef struct {
  UT_array *content;
  size_t len;
} Input;

Input * input_new(UT_array *content, size_t len) {
  Input *input = malloc(sizeof(Input));
  *input = (Input) {
    .content = content,
    .len = len,
  };
  return input;
}

void input_free(Input *input){
  utarray_free(input->content);
  free(input);  
}

Input* read_input_file() {
    char input_file_path[MAX_PATH_LENGTH];

    snprintf(input_file_path, MAX_PATH_LENGTH, "%s/%s", PROJECT_ROOT_DIR, "resources/" DAY_NUMBER "/" INPUT_FILE_NAME);
    FILE *input_file;
    char buffer[BUFFER_SIZE];
    printf("Reading file: %s\n", input_file_path);

    input_file = fopen(input_file_path, "r");
    if (input_file == NULL) {
        perror("Error reading file");
        return NULL;
    }

    UT_array *grid_input;
    utarray_new(grid_input, &ut_int_icd); 
    size_t len = 0;
    while (fgets(buffer, BUFFER_SIZE, input_file) != NULL) {
        size_t len = strcspn(buffer, "\n");
        if (len == 0) {
            continue;
        }

        len += 1;
    }

    fclose(input_file);

    return NULL;
}

unsigned long solution1(void* content) {
    return 0;  
}

unsigned long solution2(void* content) {
    return 0;  
}

int main() {
    Input *content = read_input_file();
    if (content == NULL) {
        return 1;
    }
    unsigned long res1 = solution1(content);
    printf("Solution 1: %lu\n", res1);
    
    content = read_input_file();
    unsigned long res2 = solution2(content);
    printf("Solution 2: %lu\n", res2);

    input_free(content);
    return 0;
}
