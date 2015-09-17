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

static void calculateVerticalTerrainLine(
  /*out*/ int &bottom_terrain_height,
  /*out*/ int &top_terrain_height,
  int area_height,
  int max_passage_height,
  int min_passage_height,
  Vector2i top_steepness,
  Vector2i bottom_steepness
)
{
  int oldBottomHeight = bottom_terrain_height;
  int oldTopHeight    = top_terrain_height;

  // generating bottom terrain
  bottom_steepness.x = std::min<int>(oldBottomHeight - 1, bottom_steepness.x);
  
  bottom_terrain_height = std::min<int>(
    ::rand() % (bottom_steepness.x + bottom_steepness.y + 1) + bottom_terrain_height - bottom_steepness.x,
    area_height - oldTopHeight - min_passage_height - (oldTopHeight ? 0 : 1)
  );

  // generating top terrain
  int spaceLeft = area_height - bottom_terrain_height;
  top_steepness.x = std::min<int>(oldTopHeight - 1, top_steepness.x);
  
  log("STEEPNESS %d %d", top_steepness.x, top_steepness.y);
  
  top_terrain_height = std::min<int>(
    ::rand() % (top_steepness.x + top_steepness.y + 1) + top_terrain_height - top_steepness.x,
    std::min<int>(
      area_height - oldBottomHeight - min_passage_height,
      spaceLeft - min_passage_height
    )
  );
  log("FOO");
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
  int   from_top_height    = 0,
  int   from_bottom_height = 0,
  int   min_passage_height = 1,
  int   top_steepness      = 2,
  int   bottom_steepness   = 2
)
{
  int areaHeight = area->getHeightInBlocks();
  int bottomTerrainHeight = from_bottom_height;
  int topTerrainHeight = from_top_height;
  
  // generating first column
  if (bottomTerrainHeight && topTerrainHeight) {  
    calculateVerticalTerrainLine(
      bottomTerrainHeight,
      topTerrainHeight,
      areaHeight,
      max_passage_height,
      min_passage_height,
      Vector2i(top_steepness, top_steepness),
      Vector2i(bottom_steepness, bottom_steepness)
    );
  } else {
    bottomTerrainHeight = ::rand() % (areaHeight - min_passage_height - 1) + 1;
    topTerrainHeight    = ::rand() % (areaHeight - bottomTerrainHeight - min_passage_height) + 1;
  }

  assert(bottomTerrainHeight);
  assert(topTerrainHeight);
  
  fillVerticalTerrainLine(area, 0, topTerrainHeight, bottomTerrainHeight);
  
  area->intAttributes["left_exit_top_terrain_height"]    = topTerrainHeight;
  area->intAttributes["left_exit_bottom_terrain_height"] = bottomTerrainHeight;
  
  // generating the rest of the columns
  for (int i = 1; i < area->getWidthInBlocks(); ++i) {  
    calculateVerticalTerrainLine(
      bottomTerrainHeight,
      topTerrainHeight,
      areaHeight,
      max_passage_height,
      min_passage_height,
      Vector2i(top_steepness, top_steepness),
      Vector2i(bottom_steepness, bottom_steepness)
    );
    
    assert(bottomTerrainHeight);
    assert(topTerrainHeight);
    
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
//  Area *adjancentRight  = NULL;
//  Area *adjancentTop    = NULL;
//  Area *adjancentBottom = NULL;
  int fromTopTerrainHeight    = 0;
  int fromBottomTerrainHeight = 0;
  
  try {
    adjancentLeft = map.at(std::pair<int,int>(area_x - 1, area_y));
    fromTopTerrainHeight = adjancentLeft->intAttributes.at("right_exit_top_terrain_height");
    fromBottomTerrainHeight = adjancentLeft->intAttributes.at("right_exit_bottom_terrain_height");
  } catch (std::out_of_range &e) {}
  
 /* try {
    adjancentRight = map.at(std::pair<int,int>(area_x + 1, area_y));
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentTop = map.at(std::pair<int,int>(area_x, area_y - 1));
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentBottom = map.at(std::pair<int,int>(area_x, area_y + 1));
  } catch (std::out_of_range &e) {}
*/ 
  log("FROM TOP: %d, FROM BOTTOM: %d", fromTopTerrainHeight, fromBottomTerrainHeight); 
  fillHorizontalPassage(area, area->getHeightInBlocks() - 2, fromTopTerrainHeight, fromBottomTerrainHeight);
}
