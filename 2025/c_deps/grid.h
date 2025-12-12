#include "utarray.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    size_t x;
    size_t y;
} Coords;

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

MoveDirectionResult grid_move_direction(Coords *coords_out, char *content_out, Grid *grid, Coords *coords, Direction direction, unsigned int amount) {
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

void grid_replace_all(Grid *grid, char val, char with) {
    UT_array **current_row_ptr = (UT_array**)utarray_front(grid->content);
    while (current_row_ptr != NULL) {
        UT_array *current_row = *current_row_ptr; 
        int *char_val = (int*)utarray_front(current_row);
        while (char_val != NULL) {
            if (*char_val == val)
            {
                *char_val = (int)with;
            }
            char_val = (int*)utarray_next(current_row, char_val);
        }
        current_row_ptr = (UT_array**)utarray_next(grid->content, current_row_ptr);
    }
    
}

