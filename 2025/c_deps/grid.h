#include "utarray.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t x;
  size_t y;
} GridCoords;

static UT_icd grid_coords_icd = {sizeof(GridCoords), NULL, NULL, NULL};

typedef enum {
  GRID_DIRECTION_NORTH,
  GRID_DIRECTION_NORTH_EAST,
  GRID_DIRECTION_EAST,
  GRID_DIRECTION_SOUTH_EAST,
  GRID_DIRECTION_SOUTH,
  GRID_DIRECTION_SOUTH_WEST,
  GRID_DIRECTION_WEST,
  GRID_DIRECTION_NORTH_WEST,
} GridDirection;

#define GRID_ALL_DIRECTIONS_LEN 8
static GridDirection *all_directions() {
  GridDirection *directions =
      (GridDirection *)malloc(GRID_ALL_DIRECTIONS_LEN * sizeof(GridDirection));
  if (directions == NULL) {
    return NULL;
  }

  directions[0] = GRID_DIRECTION_NORTH;
  directions[1] = GRID_DIRECTION_NORTH_EAST;
  directions[2] = GRID_DIRECTION_EAST;
  directions[3] = GRID_DIRECTION_SOUTH_EAST;
  directions[4] = GRID_DIRECTION_SOUTH;
  directions[5] = GRID_DIRECTION_SOUTH_WEST;
  directions[6] = GRID_DIRECTION_WEST;
  directions[7] = GRID_DIRECTION_NORTH_WEST;

  return directions;
}

typedef struct {
  UT_array *content;
  size_t width;
  size_t height;
} Grid;

static Grid *grid_new(UT_array *content, size_t width, size_t height) {
  Grid *grid;
  grid = (Grid *)malloc(sizeof(Grid));
  *grid = (Grid){
      .content = content,
      .width = width,
      .height = height,
  };
  return grid;
}

static Grid *grid_new_fill(size_t width, size_t height, char elem) {
  Grid *grid;
  grid = (Grid *)malloc(sizeof(Grid));
  UT_array *content;
  utarray_new(content, &ut_ptr_icd);
  for (size_t y = 0; y < height; ++y) {
    UT_array *row;
    utarray_new(row, &ut_int_icd);
    for (size_t x = 0; x < width; ++x) {
      int val = elem;
      utarray_push_back(row, &val);
    }
    utarray_push_back(content, row);
  }
  *grid = (Grid){
      .content = content,
      .width = width,
      .height = height,
  };
  return grid;
}
static void grid_print(Grid *grid) {
  UT_array **current_row_ptr = (UT_array **)utarray_front(grid->content);
  while (current_row_ptr != NULL) {
    UT_array *current_row = *current_row_ptr;
    int *char_val = (int *)utarray_front(current_row);
    while (char_val != NULL) {
      printf("%c", (char)*char_val);
      char_val = (int *)utarray_next(current_row, char_val);
    }
    printf("\n");
    current_row_ptr = (UT_array **)utarray_next(grid->content, current_row_ptr);
  }
}

static void grid_free(Grid *grid) {
  UT_array *content = grid->content;
  UT_array **p = (UT_array **)utarray_front(content);
  while (p != NULL) {
    utarray_free(*p);
    p = (UT_array **)utarray_next(content, p);
  }
  utarray_free(content);
}

static char grid_get_unchecked(Grid *grid, GridCoords *coords) {
  UT_array **row = (UT_array **)utarray_eltptr(grid->content, coords->y);
  char *elem = (char *)utarray_eltptr(*row, coords->x);
  if (elem == NULL) {
    return '\0';
  }
  return *elem;
}

static char *grid_get_ptr_unchecked(Grid *grid, GridCoords *coords) {
  UT_array **row = (UT_array **)utarray_eltptr(grid->content, coords->y);
  char *elem = (char *)utarray_eltptr(*row, coords->x);
  return elem;
}

typedef enum {
  MOVE_SUCCESS,
  MOVE_OUT_OF_BOUNDS,
} MoveDirectionResult;

static MoveDirectionResult grid_move_direction(GridCoords *coords_out,
                                               char *content_out, Grid *grid,
                                               GridCoords *coords,
                                               GridDirection direction,
                                               unsigned int amount) {
  *coords_out = (GridCoords){
      .x = coords->x,
      .y = coords->y,
  };
  switch (direction) {
  case GRID_DIRECTION_NORTH:
    if (amount > coords->y) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->y -= amount;
    break;
  case GRID_DIRECTION_NORTH_EAST:
    if (amount > grid->width - coords->x - 1 || amount > coords->y) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->x += amount;
    coords_out->y -= amount;
    break;
  case GRID_DIRECTION_EAST:
    if (amount > grid->width - coords->x - 1) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->x += amount;
    break;
  case GRID_DIRECTION_SOUTH_EAST:
    if (amount > grid->width - coords->x - 1 ||
        amount > grid->height - coords->y - 1) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->x += amount;
    coords_out->y += amount;
    break;
  case GRID_DIRECTION_SOUTH:
    if (amount > grid->height - coords->y - 1) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->y += amount;
    break;
  case GRID_DIRECTION_SOUTH_WEST:
    if (amount > coords->x || amount > grid->height - coords->y - 1) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->x -= amount;
    coords_out->y += amount;
    break;
  case GRID_DIRECTION_WEST:
    if (amount > coords->x) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->x -= amount;
    break;
  case GRID_DIRECTION_NORTH_WEST:
    if (amount > coords->x || amount > coords->y) {
      return MOVE_OUT_OF_BOUNDS;
    }
    coords_out->x -= amount;
    coords_out->y -= amount;
    break;
  }
  *content_out = grid_get_unchecked(grid, coords_out);
  return MOVE_SUCCESS;
}

static void grid_replace_all(Grid *grid, char val, char with) {
  UT_array **current_row_ptr = (UT_array **)utarray_front(grid->content);
  while (current_row_ptr != NULL) {
    UT_array *current_row = *current_row_ptr;
    int *char_val = (int *)utarray_front(current_row);
    while (char_val != NULL) {
      if (*char_val == val) {
        *char_val = (int)with;
      }
      char_val = (int *)utarray_next(current_row, char_val);
    }
    current_row_ptr = (UT_array **)utarray_next(grid->content, current_row_ptr);
  }
}
