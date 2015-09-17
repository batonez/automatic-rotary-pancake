#pragma once

#include <strug/Level.h>

class WorldGenerator
{
  public:
    enum AreaType
    {
      PASSAGE_HORIZONTAL,
      PASSAGE_VERTICAL,
      PASSAGE_LEFT_TO_BOTTOM,
      PASSAGE_LEFT_TO_TOP,
      PASSAGE_BOTTOM_TO_RIGHT,
      PASSAGE_TOP_TO_RIGHT
    };
    
  private:
    long seed;
    
  public:
    WorldGenerator(const long seed_param = 0);
    void setSeed(const long seed_param = 0);
    void fillArea(Area *area, AreaMap &map, int area_x, int area_y, AreaType type);
};
