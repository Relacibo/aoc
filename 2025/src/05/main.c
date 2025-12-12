#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "../../c_deps/utarray.h"

#define MAX_PATH_LENGTH  100
#define BUFFER_SIZE  512

#define DAY_NUMBER "05"
#define INPUT_FILE_NAME "input"
// #define INPUT_FILE_NAME "example"

#ifndef PROJECT_ROOT_DIR
#define PROJECT_ROOT_DIR "."
#endif

#define DEBUG_PRINT_SOLUTION_1 0
#define DEBUG_PRINT_SOLUTION_2 1

UT_icd long_icd = {sizeof(long), NULL, NULL, NULL};

typedef struct {
    long from;
    long to;
} Interval;

UT_icd interval_icd = {sizeof(Interval), NULL, NULL, NULL};

typedef struct {
  UT_array *fresh_ids;
  UT_array *ids;
} Input;

Input * input_new(UT_array *fresh_ids, UT_array *ids ) {
    Input *input = malloc(sizeof(Input));
    *input = (Input) {
        .fresh_ids = fresh_ids,
        .ids = ids,
    };
    return input;
}

void input_free(Input *input){
    utarray_free(input->fresh_ids);
    utarray_free(input->ids);
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

    UT_array *fresh_ids;
    utarray_new(fresh_ids, &interval_icd);

    while (fgets(buffer, BUFFER_SIZE, input_file) != NULL) {
        Interval interval;
        int success = sscanf(buffer, "%ld-%ld", &interval.from, &interval.to);
        if (success != 2) {
            break;
        }
        utarray_push_back(fresh_ids, &interval);
    }


    UT_array *ids;
    utarray_new(ids, &long_icd);
    while (fgets(buffer, BUFFER_SIZE, input_file) != NULL) {
        long id;
        int success = sscanf(buffer, "%ld", &id);
        if (success != 1) {
            break;
        }
        utarray_push_back(ids, &id);
    }

    fclose(input_file);

    return input_new(fresh_ids, ids );
}

unsigned long solution1(Input *content) {
    unsigned long sum = 0;
    long *id = utarray_front(content->ids);
    while (id != NULL) {
    Interval *fresh_id = utarray_front(content->fresh_ids);
        while (fresh_id != NULL)   {
            if (*id >= fresh_id->from && *id <= fresh_id->to) {
                if (DEBUG_PRINT_SOLUTION_1) {
                   printf("%ld in range %ld-%ld\n", *id, fresh_id->from, fresh_id->to);
                } 
                sum += 1;
                break;
            } else {
               if (DEBUG_PRINT_SOLUTION_1) {
                   printf("%ld not in range %ld-%ld\n", *id, fresh_id->from, fresh_id->to);
               } 
            }
            fresh_id = utarray_next(content->fresh_ids, fresh_id);
        }
        id = utarray_next(content->ids, id);
    }
    return sum;  
}

unsigned long solution2(Input *input) {
    UT_array *ranges = input->fresh_ids;
    unsigned long sum = 0;
    Interval *range_cp = utarray_back(ranges);
    while (range_cp != NULL) {
        Interval range;
        memcpy(&range, range_cp, sizeof(Interval));
        utarray_pop_back(ranges);
        int has_merged = 0;
        Interval *other = utarray_back(ranges);
        while(other != NULL){
            int from_inside = range.from >= other->from && range.from <= other->to;
            int to_inside = range.to >= other->from && range.to <= other->to;
            if (from_inside || to_inside) {
                if (DEBUG_PRINT_SOLUTION_2) {
                    printf("Merging %ld-%ld into %ld-%ld ", range.from, range.to, other->from, other->to);
                }
                if (from_inside && !to_inside) {
                    if (DEBUG_PRINT_SOLUTION_2) {
                        printf("(Extended upwards)\n");
                    }
                    other->to = range.to;
                } else if (!from_inside && to_inside) {
                    if (DEBUG_PRINT_SOLUTION_2) {
                        printf("(Extended downwards)\n");
                    }
                    other->from = range.from;
                } else if (DEBUG_PRINT_SOLUTION_2) {
                    printf("(Consumed)\n");
                }
                has_merged = 1;
                break;
            } else if (other->from >= range.from
                    && other->from <= range.to
                    && other->to >= range.from
                    && other->to <= range.to) {
                *other = (Interval) {
                    .from = range.from,
                    .to = range.to,
                };
                if (DEBUG_PRINT_SOLUTION_2) {
                    printf("Merging %ld-%ld into %ld-%ld ", range.from, range.to, other->from, other->to);
                    printf("(Replacing)\n");
                }
                has_merged = 1;
                break;
            }
            other = utarray_prev(ranges, other);
        }
        if (!has_merged) {
            sum += range.to - range.from + 1;
            if (DEBUG_PRINT_SOLUTION_2) {
                printf("Can not be merged: %ld-%ld\n", range.from, range.to);
            }
        }
        range_cp = utarray_back(ranges);
    }
    return sum;
}

int main() {
    Input *content = read_input_file();
    if (content == NULL) {
        return 1;
    }
    unsigned long res1 = solution1(content);
    printf("Solution 1: %lu\n", res1);
    
    unsigned long res2 = solution2(content);
    printf("Solution 2: %lu\n", res2);

    input_free(content);
    return 0;
}
