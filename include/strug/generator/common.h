#pragma once

#include <strug/Area.h>

#ifdef DEBUG_GENERATOR
#define GENERATOR_LOG(...) log(...)
#else
#define GENERATOR_LOG(...)
#endif

int not_negative(int val);

void fill_stripe(
  Area *area,
  int  col_index,
  int  top_terrain_height,
  int  bottom_terrain_height,
  bool transpose = false,
  bool invert_x = false,
  bool invert_y = false
);

Vector2i get_constrained_steepness(
  int  area_width,
  int  min_height_this_col,
  int  max_height_this_col,
  int  from_height,
  int  to_height,
  int  current_stripe_number,
  int  max_steepness,
  bool exact_destination = false
);