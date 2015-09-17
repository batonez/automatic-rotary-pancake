#include <glade/debug/log.h>
#include <glade/math/vector.h>
#include <strug/WorldGenerator.h>
#include <strug/Level.h>
#include <strug/blocks/Terrain.h>

#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <assert.h>

// Area generation algorigthms: ================================================

static void fillRandom(Area *area)
{
  for (int j = 0; j < area->getHeightInBlocks(); ++j) {
    for (int i = 0; i < area->getWidthInBlocks(); ++i) {
      if (::rand() % 2) {
        area->add(new Terrain(), i, j);
      }
    }
  }
}

static void fillHorizontalSymmetricalPassage(Area *area, int maxPassageHeight, int minPassageHeight = 1)
{
  for (int i = 0; i < area->getWidthInBlocks(); ++i) {
    int passageHeight = ::rand() % (maxPassageHeight - minPassageHeight) + minPassageHeight;
    int numberOfTerrainBlocks = area->getHeightInBlocks() - passageHeight;
    int topTerrainHeight = numberOfTerrainBlocks / 2;
    int bottomTerrainHeight = numberOfTerrainBlocks - topTerrainHeight;
        
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
      if (j < topTerrainHeight || j >= area->getHeightInBlocks() - bottomTerrainHeight) {
        area->add(new Terrain(), i, j);
      }
    }
  }
}

static void fillHorizontalSymmetricalPassageWithSteepness(Area *area, int maxPassageHeight, int minPassageHeight = 1, int steepness = 3)
{
  int previousPassageHeight = 0;
  int passageHeight;
  
  for (int i = 0; i < area->getWidthInBlocks(); ++i) {
    if (!previousPassageHeight) {
      passageHeight = ::rand() % (maxPassageHeight - minPassageHeight) + minPassageHeight;
    } else {
      int range;
      
      if (previousPassageHeight - steepness < minPassageHeight) {
        range = steepness + 1;
      } else {
        range = steepness * 2 + 1;
      }
      
      passageHeight = std::min<int>(
        ::rand() % (range)
        + std::max<int>(
          previousPassageHeight - steepness, minPassageHeight
        ),
        maxPassageHeight
      );
    }
    
    int numberOfTerrainBlocks = area->getHeightInBlocks() - passageHeight;
    int topTerrainHeight = numberOfTerrainBlocks / 2;
    int bottomTerrainHeight = numberOfTerrainBlocks - topTerrainHeight;
        
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
      if (j < topTerrainHeight || j >= area->getHeightInBlocks() - bottomTerrainHeight) {
        area->add(new Terrain(), i, j);
      }
    }
    
    previousPassageHeight = passageHeight;
  }
}

//==============================================================================

Vector2i getConstrainedSteepness(int area_width, int min_height_this_col, int max_height_this_col, int from_height, int to_height, int current_area_x, int max_steepness)
{
  Vector2i result(max_steepness, max_steepness);
  
  // Cannot let the passage be taller than max passage height  
  result.x = std::min<int>(from_height - min_height_this_col, result.x);  
  // Exclude corner blocking
  result.y = std::min<int>(max_height_this_col - from_height, result.y);
  
  if (!from_height || !to_height) {
    return result;
  }
  
  int colsLeft = area_width - current_area_x;
  int maxFinalHeight = from_height + colsLeft * max_steepness;
  int minFinalHeight = from_height - colsLeft * max_steepness;
  int maxRequiredFinalHeight = to_height + max_steepness;
  int minRequiredFinalHeight = to_height - max_steepness;
  
  if (maxFinalHeight <=  maxRequiredFinalHeight) {
    result.x = -result.y;
  }
  
  if (minFinalHeight >= minRequiredFinalHeight) {
    result.y = -result.x;
  }
  
  return result;
}

static void calculateVerticalTerrainLine(
  /*in-out*/ int &bottom_terrain_height,
  /*in-out*/ int &top_terrain_height,
  int to_bottom_terrain_height,
  int to_top_terrain_height,
  int area_width,
  int area_height,
  int current_area_x,
  int max_passage_height,
  int min_passage_height,
  int max_top_steepness,
  int max_bottom_steepness
)
{
  assert(bottom_terrain_height > 0);
  assert(top_terrain_height > 0);

  log("TERRAIN BOTTOM: %d, TOP: %d", bottom_terrain_height, top_terrain_height);
  
  int oldBottomHeight = bottom_terrain_height;
  int oldTopHeight    = top_terrain_height;
  
  // Min and max passage height
  int maxHeightThisCol = area_height - oldTopHeight - min_passage_height;
  
  if (area_width - current_area_x == 1 && to_bottom_terrain_height) {
    maxHeightThisCol = std::min<int>(area_height - to_top_terrain_height - min_passage_height, maxHeightThisCol);
  }
  
  int minHeightThisCol = 1;
  
  // generating bottom terrain
  Vector2i bottomSteepness = getConstrainedSteepness(area_width, minHeightThisCol, maxHeightThisCol, oldBottomHeight, to_bottom_terrain_height, current_area_x, max_bottom_steepness);

  bottom_terrain_height = 
    ::rand() % (bottomSteepness.x + bottomSteepness.y + 1) + oldBottomHeight - bottomSteepness.x;
  
  int spaceLeft = area_height - bottom_terrain_height;
  
  // Min and max passage height
  maxHeightThisCol = std::min<int>(
    // corner blocking with the left neighbor col
    area_height - oldBottomHeight - min_passage_height, 
    spaceLeft - min_passage_height
  );
  
   // corner blocking with the left neighbor col
  if (area_width - current_area_x == 1 && to_top_terrain_height) {
    maxHeightThisCol = std::min<int>(area_height - to_bottom_terrain_height - min_passage_height, maxHeightThisCol);
  }
  
  minHeightThisCol = std::max<int>(spaceLeft - max_passage_height, 1);
  
  // generating top terrain
  Vector2i topSteepness = getConstrainedSteepness(area_width, minHeightThisCol, maxHeightThisCol, oldTopHeight, to_top_terrain_height, current_area_x, max_top_steepness);

  // reimplement this as steepness constrain
  top_terrain_height = 
    ::rand() % (topSteepness.x + topSteepness.y + 1) + oldTopHeight - topSteepness.x;
}

static void fillVerticalTerrainLine(
  Area *area,
  int  col_index,
  int  top_terrain_height,
  int  bottom_terrain_height
)
{
  for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
    if (j < top_terrain_height || j >= area->getHeightInBlocks() - bottom_terrain_height) {
        area->add(new Terrain(), col_index, j);
    }
  }
}

static void fillHorizontalPassage(
  Area *area,
  int   max_passage_height,
  int   min_passage_height = 1,
  int   from_top_height    = 0,
  int   from_bottom_height = 0,
  int   to_top_height    = 0,
  int   to_bottom_height = 0,
  int   max_top_steepness      = 2,
  int   max_bottom_steepness   = 2
)
{
  int areaHeight = area->getHeightInBlocks();
  int areaWidth = area->getWidthInBlocks();
  int bottomTerrainHeight = from_bottom_height;
  int topTerrainHeight = from_top_height;
  
  // generating first column
  if (bottomTerrainHeight && topTerrainHeight) {  
    calculateVerticalTerrainLine(
      bottomTerrainHeight,
      topTerrainHeight,
      to_bottom_height,
      to_top_height,
      areaWidth,
      areaHeight,
      0,
      max_passage_height,
      min_passage_height,
      max_top_steepness,
      max_bottom_steepness
    );
  } else {
    bottomTerrainHeight = ::rand() % (areaHeight - max_passage_height - 1) + 1;
    int spaceLeft = areaHeight - bottomTerrainHeight;
    log("SPACE LEFT: %d, MAX PASSAGE HEIGHT: %d", spaceLeft, max_passage_height);
    topTerrainHeight    = ::rand() % (max_passage_height - min_passage_height) + spaceLeft - max_passage_height;
  }

  fillVerticalTerrainLine(area, 0, topTerrainHeight, bottomTerrainHeight);
  
  area->intAttributes["left_exit_top_terrain_height"]    = topTerrainHeight;
  area->intAttributes["left_exit_bottom_terrain_height"] = bottomTerrainHeight;
  
  // generating the rest of the columns
  for (int i = 1; i < area->getWidthInBlocks(); ++i) {  
    calculateVerticalTerrainLine(
      bottomTerrainHeight,
      topTerrainHeight,
      to_bottom_height,
      to_top_height,
      areaWidth,
      areaHeight,
      i,
      max_passage_height,
      min_passage_height,
      max_top_steepness,
      max_bottom_steepness
    );
    
    fillVerticalTerrainLine(area, i, topTerrainHeight, bottomTerrainHeight);
  }
  
  area->intAttributes["right_exit_top_terrain_height"]    = topTerrainHeight;
  area->intAttributes["right_exit_bottom_terrain_height"] = bottomTerrainHeight;
}

//==============================================================================

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
//  Area *adjancentTop    = NULL;
//  Area *adjancentBottom = NULL;
  int fromTopTerrainHeight    = 0;
  int fromBottomTerrainHeight = 0;
  int toTopTerrainHeight      = 0;
  int toBottomTerrainHeight   = 0;
  
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
/*  
  try {
    adjancentTop = map.at(std::pair<int,int>(area_x, area_y - 1));
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentBottom = map.at(std::pair<int,int>(area_x, area_y + 1));
  } catch (std::out_of_range &e) {}
*/ 

  fillHorizontalPassage(
    area,
    //area->getHeightInBlocks() - 2,
    5,
    3,
    fromTopTerrainHeight,
    fromBottomTerrainHeight,
    toTopTerrainHeight,
    toBottomTerrainHeight
  );
}
