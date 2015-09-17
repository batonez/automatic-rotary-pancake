#pragma once

#include <strug/Level.h>

class WorldGenerator
{
  public:
    enum AreaType
    {
      // no passage
      AREA_FULL,
      // straight
      PASSAGE_HORIZONTAL,
      PASSAGE_VERTICAL,
      // Turns
      PASSAGE_LEFT_TO_BOTTOM, PASSAGE_BOTTOM_TO_LEFT,
      PASSAGE_LEFT_TO_TOP, PASSAGE_TOP_TO_LEFT,
      PASSAGE_BOTTOM_TO_RIGHT, PASSAGE_RIGHT_TO_BOTTOM,
      PASSAGE_TOP_TO_RIGHT, PASSAGE_RIGHT_TO_TOP,
      // Intersections
      PASSAGE_TCROSS_BLIND_TOP,
      PASSAGE_TCROSS_BLIND_BOTTOM,
      PASSAGE_TCROSS_BLIND_LEFT,
      PASSAGE_TCROSS_BLIND_RIGHT,
      PASSAGE_XCROSS
    };
    
  private:
    long seed;
    
  public:
    WorldGenerator(const long seed_param = 0);
    void setSeed(const long seed_param = 0);
    void fillArea(Area *area, AreaMap &map, int area_x, int area_y, AreaType type);
};
