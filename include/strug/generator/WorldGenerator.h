#pragma once

#include <strug/Level.h>
#include <strug/generator/MazeGenerator.h>

class WorldGenerator
{
  public:
    MazeGenerator mazeGenerator;
    
  private:
    unsigned int seed;

  public:
    WorldGenerator(const unsigned int seed_param = 0);
    void setSeed(const unsigned int seed_param = 0);
    void fillArea(Area *area, int area_x, int area_y, MazeGenerator::CellType type);
    void createMazeCluster(int area_width, int area_height);
};
