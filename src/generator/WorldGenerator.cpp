#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include <limits.h>

#include <glade/debug/log.h>
#include <glade/math/vector.h>
#include <strug/Level.h>
#include <strug/blocks/Terrain.h>
#include <strug/generator/WorldGenerator.h>
#include <strug/exception/StrugException.h>
#include <strug/generator/SimpleGenerator.h>
#include <strug/generator/StraightPassage.h>
#include <strug/generator/PassageTurn.h>
#include <strug/generator/TCross.h>
#include <strug/generator/XCross.h>

#define MIN_PASSAGE_HEIGHT 4
#define MAX_PASSAGE_HEIGHT 14

static unsigned int getCombinedSeed(long seed, int x, int y)
{
  log("INT MIN: %d, INT MAX: %d, UINT_MAX: %ud", INT_MIN, INT_MAX, UINT_MAX);
  
  long long foo = 429496734297;
  int bar = (int) foo;
  log ("LL TO INT: %d", bar);
  return 0;
}

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

void WorldGenerator::createMaze()
{
  ::srand(seed);
  mazeGenerator.createMaze();
}

void WorldGenerator::fillArea(Area *area, AreaMap &map, int area_x, int area_y, AreaType type)
{
  /*::srand(*/getCombinedSeed(seed, area_x, area_y)/*)*/;
  
  area->texturePackName = "cave";

  Area *adjancentLeft   = NULL;
  Area *adjancentRight  = NULL;
  Area *adjancentTop    = NULL;
  Area *adjancentBottom = NULL;

  int leftNeighborTopHeight    = 0;
  int leftNeighborBottomHeight = 0;
  int rightNeighborTopHeight      = 0;
  int rightNeighborBottomHeight   = 0;

  int bottomNeighborLeftWidth   = 0;
  int bottomNeighborRightWidth  = 0;
  int topNeighborLeftWidth      = 0;
  int topNeighborRightWidth     = 0;
  
  try {
    adjancentLeft = map.at(std::pair<int,int>(area_x - 1, area_y));
    leftNeighborTopHeight = adjancentLeft->intAttributes.at("right_exit_top_terrain_height");
    leftNeighborBottomHeight = adjancentLeft->intAttributes.at("right_exit_bottom_terrain_height");
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentRight = map.at(std::pair<int,int>(area_x + 1, area_y));
    rightNeighborTopHeight = adjancentRight->intAttributes.at("left_exit_top_terrain_height");
    rightNeighborBottomHeight = adjancentRight->intAttributes.at("left_exit_bottom_terrain_height");
  } catch (std::out_of_range &e) {}

  try {
    adjancentTop = map.at(std::pair<int,int>(area_x, area_y - 1));
    topNeighborLeftWidth = adjancentTop->intAttributes.at("bottom_exit_left_terrain_width");
    topNeighborRightWidth = adjancentTop->intAttributes.at("bottom_exit_right_terrain_width");
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentBottom = map.at(std::pair<int,int>(area_x, area_y + 1));
    bottomNeighborLeftWidth = adjancentBottom->intAttributes.at("top_exit_left_terrain_width");
    bottomNeighborRightWidth = adjancentBottom->intAttributes.at("top_exit_right_terrain_width");
  } catch (std::out_of_range &e) {}

  switch (type)
  {
    case PASSAGE_HORIZONTAL:
      StraightPassage::createStraightPassage( 
        area,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        leftNeighborTopHeight,
        leftNeighborBottomHeight,
        rightNeighborTopHeight,
        rightNeighborBottomHeight
      );
      break;
    case PASSAGE_VERTICAL:
      StraightPassage::createStraightPassage( 
        area,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        bottomNeighborLeftWidth,
        bottomNeighborRightWidth,
        topNeighborLeftWidth,
        topNeighborRightWidth
      );
      break;
    case PASSAGE_LEFT_TO_BOTTOM:
    case PASSAGE_BOTTOM_TO_LEFT:
      PassageTurn::createPassageTurn(
        area,
        false,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        leftNeighborTopHeight,
        leftNeighborBottomHeight,
        bottomNeighborLeftWidth,
        bottomNeighborRightWidth
      );
      break;
    case PASSAGE_LEFT_TO_TOP:
    case PASSAGE_TOP_TO_LEFT:
      PassageTurn::createPassageTurn(
        area,
        false,
        false,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        leftNeighborTopHeight,
        leftNeighborBottomHeight,
        topNeighborLeftWidth,
        topNeighborRightWidth
      );
      break;
    case PASSAGE_BOTTOM_TO_RIGHT:
    case PASSAGE_RIGHT_TO_BOTTOM:
      PassageTurn::createPassageTurn(
        area,
        false,
        true,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        rightNeighborTopHeight,
        rightNeighborBottomHeight,
        bottomNeighborRightWidth,
        bottomNeighborLeftWidth
      );
      break;
    case PASSAGE_TOP_TO_RIGHT:
    case PASSAGE_RIGHT_TO_TOP:
      PassageTurn::createPassageTurn(
        area,
        true,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        topNeighborLeftWidth,
        topNeighborRightWidth,
        rightNeighborTopHeight,
        rightNeighborBottomHeight
      );
      break;
    case PASSAGE_TCROSS_BLIND_TOP:
      TCross::createPassageTurn(
        area,
        false,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        leftNeighborTopHeight,
        leftNeighborBottomHeight,
        rightNeighborTopHeight,
        rightNeighborBottomHeight,
        bottomNeighborLeftWidth,
        bottomNeighborRightWidth
      );
      break;
    case PASSAGE_TCROSS_BLIND_BOTTOM:
      TCross::createPassageTurn(
        area,
        false,
        false,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        leftNeighborTopHeight,
        leftNeighborBottomHeight,
        rightNeighborBottomHeight,
        rightNeighborTopHeight,
        topNeighborLeftWidth,
        topNeighborRightWidth
      );
      break;
    case PASSAGE_TCROSS_BLIND_RIGHT:
      TCross::createPassageTurn(
        area,
        true,
        false,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        topNeighborLeftWidth,
        topNeighborRightWidth,
        bottomNeighborRightWidth,
        bottomNeighborLeftWidth,
        leftNeighborTopHeight,
        leftNeighborBottomHeight
      );
      break;
    case PASSAGE_TCROSS_BLIND_LEFT:
      TCross::createPassageTurn(
        area,
        true,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        topNeighborLeftWidth,
        topNeighborRightWidth,
        bottomNeighborLeftWidth,
        bottomNeighborRightWidth,
        rightNeighborTopHeight,
        rightNeighborBottomHeight
      );
      break;
    case PASSAGE_XCROSS:
      XCross::createPassageTurn(
        area,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        leftNeighborTopHeight,
        leftNeighborBottomHeight,
        rightNeighborTopHeight,
        rightNeighborBottomHeight,
        bottomNeighborLeftWidth,
        bottomNeighborRightWidth,
        topNeighborLeftWidth,
        topNeighborRightWidth
      );
      break;
    case AREA_FULL:
      SimpleGenerator::fillAll(area);
      break;
    default:
      throw StrugException("Unknown passage type");
  }
}
