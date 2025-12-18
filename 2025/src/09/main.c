#include "../../c_deps/grid.h"
#include "../../c_deps/utarray.h"
#include <assert.h>
#include <limits.h>
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
#define DEBUG_PRINT_SOLUTION_2 0
#define DEBUG_PRINT_SOLUTION_2_GRID 0

typedef struct {
  long x;
  long y;
} Coords;

UT_icd coords_icd = {sizeof(Coords), NULL, NULL, NULL};

UT_array *read_input_file() {
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

  UT_array *input;
  utarray_new(input, &grid_coords_icd);
  while (fgets(buffer, BUFFER_SIZE, input_file) != NULL) {
    size_t len = strcspn(buffer, "\n");
    if (len == 0) {
      continue;
    }

    Coords coords;
    sscanf(buffer, "%ld,%ld", &coords.x, &coords.y);
    utarray_push_back(input, &coords);
  }
  fclose(input_file);

  return input;
}

unsigned long solution1() {
  UT_array *input = read_input_file();
  size_t len = utarray_len(input);
  unsigned long max = 0;
  Coords *coords_i = utarray_front(input);
  for (size_t i = 0; i < len; ++i) {
    Coords *coords_j = utarray_next(input, coords_i);
    for (size_t j = i + 1; j < len; ++j) {
      long x = coords_j->x - coords_i->x;
      long y = coords_j->y - coords_i->y;
      unsigned long area = (labs(x) + 1) * (labs(y) + 1);
      if (DEBUG_PRINT_SOLUTION_1) {
        printf("i=(%ld,%ld), ", coords_i->x, coords_i->y);
        printf("j=(%ld,%ld) ", coords_j->x, coords_j->y);
        printf("-> (%ld,%ld) -> %ld\n", x, y, area);
      }
      if (area > max) {
        if (DEBUG_PRINT_SOLUTION_1) {
          printf("New maximum\n");
        }
        max = area;
      }
      coords_j = utarray_next(input, coords_j);
    }
    coords_i = utarray_next(input, coords_i);
  }
  utarray_free(input);
  return (unsigned long)max;
}

typedef struct {
  long num;
  int winding_dir;
} Intersection;

UT_icd intersection_icd = {sizeof(Intersection), NULL, NULL, NULL};

void intersections_sort(UT_array *intersections) {
  size_t len = utarray_len(intersections);
  for (size_t i = 0; i < len; ++i) {
    long min = LONG_MAX;
    size_t i_min = -1;
    for (size_t j = i; j < len; ++j) {
      Intersection *elem = utarray_eltptr(intersections, j);
      if (elem->num < min) {
        min = elem->num;
        i_min = j;
      }
    }
    Intersection *a = utarray_eltptr(intersections, i);
    Intersection *b = utarray_eltptr(intersections, i_min);
    Intersection temp = *a;
    *a = *b;
    *b = temp;
  }
}

Grid *fill_green(UT_array *input, long x_min, long y_min, long x_max,
                 long y_max) {
  UT_array *content;
  utarray_new(content, &ut_ptr_icd);
  for (size_t y = y_min; y <= y_max; ++y) {
    if (DEBUG_PRINT_SOLUTION_2) {
      printf("y: %ld\n", y);
    }
    UT_array *y_intersections;
    utarray_new(y_intersections, &intersection_icd);
    Coords *elem1 = utarray_back(input);
    Coords *elem2 = utarray_front(input);
    while (elem2 != NULL) {
      // only look for vertical lines
      if (elem1->y == elem2->y) {
        goto step_y;
      }
      long y_smaller;
      long y_bigger;
      int top_to_bottom;
      if (elem1->y > elem2->y) {
        y_smaller = elem2->y;
        y_bigger = elem1->y;
        top_to_bottom = 0;
      } else {
        y_smaller = elem1->y;
        y_bigger = elem2->y;
        top_to_bottom = 1;
      };
      if (y_smaller <= y && y <= y_bigger) {
        int winding_dir;
        if (top_to_bottom) {
          winding_dir = -1;
        } else {
          winding_dir = 1;
        }
        Intersection intersection;
        intersection.num = elem1->x;
        intersection.winding_dir = winding_dir;
        utarray_push_back(y_intersections, &intersection);
      };
    step_y:
      elem1 = elem2;
      elem2 = utarray_next(input, elem1);
    }
    intersections_sort(y_intersections);
    int counter = 0;
    Intersection *intersection = utarray_front(y_intersections);
    UT_array *row;
    utarray_new(row, &ut_int_icd);
    for (size_t x = x_min; x <= x_max; ++x) {
      char res = '.';
      if (intersection->num == x) {
        counter += intersection->winding_dir;
        res = 'X';
      } else if (counter % 2 == 1) {
        res = 'X';
      };
      utarray_push_back(row, (int *)(&res));
    }
    utarray_free(y_intersections);
    utarray_push_back(content, &row);
  }
  Grid *grid = grid_new(content, x_max - x_min + 1, y_max - y_min + 1);
  return grid;
}

// void normalize_winding_order(UT_array *input) {
//   if (!is_inside(input, LONG_MAX, LONG_MAX)) {
//     return;
//   }

//   if (DEBUG_PRINT_SOLUTION_2) {
//     printf("Have to revert input!");
//   }

//   size_t len = utarray_len(input);
//   size_t len_half = len / 2;
//   for (size_t i = 0; i < len_half; ++i) {
//     Coords *a = utarray_eltptr(input, i);
//     Coords *b = utarray_eltptr(input, len - i - 1);
//     Coords temp = *a;
//     *a = *b;
//     *b = temp;
//   }
// }

// void print_green(UT_array *input, long x_min, long y_min, long x_max,
//                  long y_max) {
//   for (long y = y_min; y <= y_max; y++) {
//     for (long x = x_min; x <= x_max; x++) {
//       if (is_inside(input, x, y)) {
//         printf(" X");
//       } else {
//         printf(" .");
//       }
//     }
//     printf("\n");
//   }
// }
//

int intersects(Coords *a1, Coords *a2, Coords *b1, Coords *b2) { if (a1) }

int all_tiles_inside(Grid *grid, long grid_x_min, long grid_y_min,
                     Coords *coords_i, Coords *coords_j) {
  long x_min;
  long x_max;
  long y_min;
  long y_max;
  if (coords_i->x > coords_j->x) {
    x_min = coords_j->x;
    x_max = coords_i->x;
  } else {
    x_min = coords_i->x;
    x_max = coords_j->x;
  }
  if (coords_i->y > coords_j->y) {
    y_min = coords_j->y;
    y_max = coords_i->y;
  } else {
    y_min = coords_i->y;
    y_max = coords_j->y;
  }
  for (long y = y_min; y <= y_max; y++) {
    for (long x = x_min; x <= x_max; x++) {
      GridCoords c;
      c.x = x - grid_x_min;
      c.y = y - grid_y_min;
      if (grid_get_unchecked(grid, &c) != 'X') {
        return 0;
      }
    }
  }
  return 1;
}

unsigned long solution2() {
  UT_array *input = read_input_file();
  long x_min = LONG_MAX;
  long y_min = LONG_MAX;
  long x_max = LONG_MIN;
  long y_max = LONG_MIN;
  Coords *elem = utarray_front(input);
  while (elem != NULL) {
    if (elem->x < x_min) {
      x_min = elem->x;
    } else if (elem->x > x_max) {
      x_max = elem->x;
    }

    if (elem->y < y_min) {
      y_min = elem->y;
    } else if (elem->y > y_max) {
      y_max = elem->y;
    }
    elem = utarray_next(input, elem);
  }
  if (DEBUG_PRINT_SOLUTION_2) {
    printf("x_min=%ld, x_max=%ld, y_min=%ld, y_max=%ld\n", x_min, x_max, y_min,
           y_max);
  }
  Grid *grid = fill_green(input, x_min, y_min, x_max, y_max);
  if (DEBUG_PRINT_SOLUTION_2_GRID) {
    grid_print(grid);
  }
  size_t len = utarray_len(input);
  unsigned long max = 0;
  Coords *coords_i = utarray_front(input);
  for (size_t i = 0; i < len; ++i) {
    Coords *coords_j = utarray_next(input, coords_i);
    for (size_t j = i + 1; j < len; ++j) {
      long x = coords_j->x - coords_i->x;
      long y = coords_j->y - coords_i->y;
      unsigned long area = (labs(x) + 1) * (labs(y) + 1);
      if (DEBUG_PRINT_SOLUTION_2) {
        printf("i=(%ld,%ld), ", coords_i->x, coords_i->y);
        printf("j=(%ld,%ld) ", coords_j->x, coords_j->y);
        printf("-> (%ld,%ld) -> %ld\n", x, y, area);
      }
      if (area > max &&
          all_tiles_inside(grid, x_min, y_min, coords_i, coords_j)) {
        if (DEBUG_PRINT_SOLUTION_2) {
          printf("New maximum\n");
        }
        max = area;
      }
      coords_j = utarray_next(input, coords_j);
    }
    coords_i = utarray_next(input, coords_i);
  }
  utarray_free(input);
  return (unsigned long)max;
}

int main() {
  unsigned long res1 = solution1();
  printf("%ld\n", res1);
  unsigned long res2 = solution2();
  printf("%ld\n", res2);
  return 0;
}
