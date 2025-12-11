#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "../../c_deps/utarray.h"

#define MAX_PATH_LENGTH  100
#define BUFFER_SIZE  512

#ifndef PROJECT_ROOT_DIR
#define PROJECT_ROOT_DIR "."
#endif

UT_array* read_input_file() {
    char input_file_path[MAX_PATH_LENGTH];

    snprintf(input_file_path, MAX_PATH_LENGTH, "%s/%s", PROJECT_ROOT_DIR, "resources/04/input");
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
        current_row = NULL; 
    }

    if (current_row != NULL) {
        utarray_free(current_row);
    }

    fclose(input_file);
    return grid_input;
}

void print_grid(UT_array *input) {
    if (input == NULL) return;
    UT_array **current_row_ptr = (UT_array**)utarray_front(input);
    while (current_row_ptr != NULL) {
        UT_array *current_row = *current_row_ptr; 
        int *char_val = (int*)utarray_front(current_row);
        while (char_val != NULL) {
            printf("%c", (char)*char_val); 
            char_val = (int*)utarray_next(current_row, char_val);
        }
        printf("\n"); 
        current_row_ptr = (UT_array**)utarray_next(input, current_row_ptr);
    }
}

int main() {
    UT_array* input = read_input_file();

    if (input != NULL) {
        print_grid(input);

        UT_array **p = (UT_array**)utarray_front(input);
        while (p != NULL) {
            utarray_free(*p); 
            p = (UT_array**)utarray_next(input, p);
        }
        utarray_free(input); 
    }

    return 0;
}
