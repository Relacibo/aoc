#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "../../c_deps/utarray.h"

#define MAX_PATH_LENGTH  100
#define BUFFER_SIZE  512

#define DAY_NUMBER "04"
#define INPUT_FILE_NAME "input"
// #define INPUT_FILE_NAME "example"

#ifndef PROJECT_ROOT_DIR
#define PROJECT_ROOT_DIR "."
#endif

#define DEBUG_PRINT_SOLUTION_1 0
#define DEBUG_PRINT_SOLUTION_2 1

typedef enum {
    DIRECTION_NORTH,
    DIRECTION_NORTH_EAST,
    DIRECTION_EAST,
    DIRECTION_SOUTH_EAST,
    DIRECTION_SOUTH,
    DIRECTION_SOUTH_WEST,
    DIRECTION_WEST,
    DIRECTION_NORTH_WEST,
} Direction;

#define ALL_DIRECTIONS_LEN 8
Direction* all_directions() {
    Direction *directions = (Direction*)malloc(ALL_DIRECTIONS_LEN * sizeof(Direction));
    if (directions == NULL) {
        return NULL; 
    }

    directions[0] = DIRECTION_NORTH;
    directions[1] = DIRECTION_NORTH_EAST;
    directions[2] = DIRECTION_EAST;
    directions[3] = DIRECTION_SOUTH_EAST;
    directions[4] = DIRECTION_SOUTH;
    directions[5] = DIRECTION_SOUTH_WEST;
    directions[6] = DIRECTION_WEST;
    directions[7] = DIRECTION_NORTH_WEST;
    
    return directions;
}

typedef struct {
    size_t x;
    size_t y;
} Coords;

typedef struct {
    UT_array* content;
    size_t width;
    size_t height; 
} Grid;

Grid* grid_new(UT_array *content, size_t width, size_t height) {
    Grid *grid;
    grid = malloc(sizeof(Grid));
    *grid = (Grid){
        .content = content,
        .width = width,
        .height = height,
    };
    return grid;
}

void grid_print(Grid *grid) {
    UT_array **current_row_ptr = (UT_array**)utarray_front(grid->content);
    while (current_row_ptr != NULL) {
        UT_array *current_row = *current_row_ptr; 
        int *char_val = (int*)utarray_front(current_row);
        while (char_val != NULL) {
            printf("%c", (char)*char_val); 
            char_val = (int*)utarray_next(current_row, char_val);
        }
        printf("\n"); 
        current_row_ptr = (UT_array**)utarray_next(grid->content, current_row_ptr);
    }
}

void grid_free(Grid *grid) {
    UT_array *content = grid->content;
    UT_array **p = (UT_array**)utarray_front(content);
    while (p != NULL) {
        utarray_free(*p); 
        p = (UT_array**)utarray_next(content, p);
    }
    utarray_free(content); 
}

char grid_get_unchecked(Grid *grid, Coords *coords) {
    UT_array **row = (UT_array**)utarray_eltptr(grid->content, coords->y);
    char *elem = (char*)utarray_eltptr(*row, coords->x);
    if (elem == NULL) {
        return '\0';
    }
    return *elem;
}

char* grid_get_ptr_unchecked(Grid *grid, Coords *coords) {
    UT_array **row = (UT_array**)utarray_eltptr(grid->content, coords->y);
    char *elem = (char*)utarray_eltptr(*row, coords->x);
    return elem;
}

typedef enum {
    MOVE_SUCCESS,
    MOVE_OUT_OF_BOUNDS,
} MoveDirectionResult;

MoveDirectionResult move_direction(Coords *coords_out, char *content_out, Grid *grid, Coords *coords, Direction direction, unsigned int amount) {
    *coords_out = (Coords){
        .x =  coords->x,
        .y =  coords->y,
    };
    switch (direction) {
    case DIRECTION_NORTH:
        if (amount > coords->y) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->y -= amount;
        break;
    case DIRECTION_NORTH_EAST:
        if (amount > grid->width - coords->x - 1 || amount > coords->y ) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->x += amount;
        coords_out->y -= amount;
        break;
    case DIRECTION_EAST:
        if (amount > grid->width - coords->x - 1) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->x += amount;
        break;
    case DIRECTION_SOUTH_EAST:
        if (amount > grid->width - coords->x - 1 || amount > grid->height - coords->y - 1 ) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->x += amount;
        coords_out->y += amount;
        break;
    case DIRECTION_SOUTH:
        if (amount > grid->height - coords->y - 1) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->y += amount;
        break;
    case DIRECTION_SOUTH_WEST:
        if (amount > coords->x || amount > grid->height - coords->y - 1 ) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->x -= amount;
        coords_out->y += amount;
        break;
    case DIRECTION_WEST:
        if (amount > coords->x) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->x -= amount;
        break;
    case DIRECTION_NORTH_WEST:
        if (amount > coords->x || amount > coords->y ) {
            return MOVE_OUT_OF_BOUNDS;
        }
        coords_out->x -= amount;
        coords_out->y -= amount;
        break;
    }
    *content_out = grid_get_unchecked(grid, coords_out);
    return MOVE_SUCCESS;
}

Grid* read_input_file() {
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
    UT_array **current_row_ptr = (UT_array**)utarray_front(grid->content);
    size_t y = 0;
    unsigned long sum = 0;
    while (current_row_ptr != NULL) {
        size_t x = 0;
        UT_array *current_row = *current_row_ptr; 
        int *char_val = (int*)utarray_front(current_row);
        while (char_val != NULL) {
            if (*char_val == '@') {
                Coords coords;
                coords = (Coords) {
                    .x = x,
                    .y = y,
                };
                unsigned int rolls_count = 0;
                for(size_t i = 0; i < ALL_DIRECTIONS_LEN; i++) {
                    Direction direction = dirs[i];
                    Coords new_coords;
                    char val;
                    if (move_direction(&new_coords, &val, grid, &coords, direction, 1) == MOVE_SUCCESS) {
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
            char_val = (int*)utarray_next(current_row, char_val);
            x++;
        }
        current_row_ptr = (UT_array**)utarray_next(grid->content, current_row_ptr);
        y++;
    }
    free(dirs);
    return sum;
}

void cleanup_marked(Grid *grid) {
    UT_array **current_row_ptr = (UT_array**)utarray_front(grid->content);
    while (current_row_ptr != NULL) {
        UT_array *current_row = *current_row_ptr; 
        int *char_val = (int*)utarray_front(current_row);
        while (char_val != NULL) {
            if (*char_val == 'x')
            {
                *char_val = (int)'.';
            }
            char_val = (int*)utarray_next(current_row, char_val);
        }
        current_row_ptr = (UT_array**)utarray_next(grid->content, current_row_ptr);
    }
    
}

unsigned long solution2(Grid *grid) {
    Direction *dirs = all_directions();
    unsigned long sum = 0;
    unsigned int step = 1;
    unsigned long removed_last_round = 1;
    while (removed_last_round > 0) {
        removed_last_round = 0;
        size_t y = 0;
        UT_array **current_row_ptr = (UT_array**)utarray_front(grid->content);
        while (current_row_ptr != NULL) {
            size_t x = 0;
            UT_array *current_row = *current_row_ptr; 
            int *char_val = (int*)utarray_front(current_row);
            while (char_val != NULL) {
                if (*char_val == '@') {
                    Coords coords;
                    coords = (Coords) {
                        .x = x,
                        .y = y,
                    };
                    unsigned int rolls_count = 0;
                    for(size_t i = 0; i < ALL_DIRECTIONS_LEN; i++) {
                        Direction direction = dirs[i];
                        Coords new_coords;
                        char val;
                        if (move_direction(&new_coords, &val, grid, &coords, direction, 1) == MOVE_SUCCESS) {
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
                char_val = (int*)utarray_next(current_row, char_val);
                x++;
            }
            current_row_ptr = (UT_array**)utarray_next(grid->content, current_row_ptr);
            y++;
        }
        if (DEBUG_PRINT_SOLUTION_2) {
            printf("\n");
            printf("Step %d:\n", step);
            grid_print(grid);
        }
        cleanup_marked(grid);
        step++;
    }
    free(dirs);
    return sum;
}


int main() {
    Grid* grid = read_input_file();
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
