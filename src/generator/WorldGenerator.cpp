#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <assert.h>

#include <glade/debug/log.h>
#include <glade/math/vector.h>
#include <strug/WorldGenerator.h>
#include <strug/Level.h>
#include <strug/blocks/Terrain.h>
#include <strug/exception/StrugException.h>
#include <strug/generator/StraightPassage.h>
#include <strug/generator/PassageTurn.h>

WorldGenerator::WorldGenerator(long seed_param)
{
  setSeed(seed_param);
}

void WorldGenerator::setSeed(long seed_param)
{
  if (!seed_param) {
    seed = ::time(NULL);
  }
  
  log("World generator seed is %ld", seed);
  ::srand(seed);
}

void WorldGenerator::fillArea(Area *area, AreaMap &map, int area_x, int area_y)
{
  area->texturePackName = "cave";

  Area *adjancentLeft   = NULL;
  Area *adjancentRight  = NULL;
  Area *adjancentTop    = NULL;
  Area *adjancentBottom = NULL;

  int fromTopTerrainHeight    = 0;
  int fromBottomTerrainHeight = 0;
  int toTopTerrainHeight      = 0;
  int toBottomTerrainHeight   = 0;

  int fromLeftTerrainWidth    = 0;
  int fromRightTerrainWidth   = 0;
  int toLeftTerrainWidth      = 0;
  int toRightTerrainWidth     = 0;
 
  try {
    adjancentLeft = map.at(std::pair<int,int>(area_x - 1, area_y));
    fromTopTerrainHeight = adjancentLeft->intAttributes.at("right_exit_top_terrain_height");
    fromBottomTerrainHeight = adjancentLeft->intAttributes.at("right_exit_bottom_terrain_height");
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentRight = map.at(std::pair<int,int>(area_x + 1, area_y));
    toTopTerrainHeight = adjancentRight->intAttributes.at("left_exit_top_terrain_height");
    toBottomTerrainHeight = adjancentRight->intAttributes.at("left_exit_bottom_terrain_height");
  } catch (std::out_of_range &e) {}

  try {
    adjancentTop = map.at(std::pair<int,int>(area_x, area_y - 1));
    toLeftTerrainWidth = adjancentTop->intAttributes.at("bottom_exit_left_terrain_width");
    toRightTerrainWidth = adjancentTop->intAttributes.at("bottom_exit_right_terrain_width");
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentBottom = map.at(std::pair<int,int>(area_x, area_y + 1));
    fromLeftTerrainWidth = adjancentBottom->intAttributes.at("top_exit_left_terrain_width");
    fromRightTerrainWidth = adjancentBottom->intAttributes.at("top_exit_right_terrain_width");
  } catch (std::out_of_range &e) {}

  PassageTurn::createStraightPassage( 
    area,
    true,
    area->getHeightInBlocks() / 2,
    3,
    fromTopTerrainHeight,
    fromBottomTerrainHeight,
    8,
    2
   
   // fromLeftTerrainWidth,
   // fromRightTerrainWidth,
   // toLeftTerrainWidth,
   // toRightTerr ainWidth
  );
}
