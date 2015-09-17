#pragma once

#include <strug/Level.h>

class TCross
{
  public:
    static void createPassageTurn(
      Area *area,
      bool  transpose,
      bool  invert_x,
      bool  invert_y,
      int   max_passage_thickness,
      int   min_passage_thickness = 1,
      int   from_top_height       = 0,
      int   from_bottom_height    = 0,
      int   to_top_height         = 0,
      int   to_bottom_height      = 0,
      int   to_left_width         = 0,
      int   to_right_width        = 0,
      int   max_top_steepness     = 2,
      int   max_bottom_steepness  = 2
    );
    
  private:
    static void calculatePassageTurnStripe(
      /*in-out*/ int &bottom_terrain_height,
      /*in-out*/ int &top_terrain_height,
      int to_bottom_terrain_height,
      int to_top_terrain_height,
      int to_left_width,
      int to_right_width,
      int area_width,
      int area_height,
      int current_stripe_number,
      int max_passage_thickness,
      int min_passage_thickness,
      int max_top_steepness,
      int max_bottom_steepness
    );
};