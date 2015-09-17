#include <glade/debug/log.h>
#include <strug/WorldGenerator.h>
#include <strug/Level.h>
#include <strug/blocks/Terrain.h>

#include <stdlib.h>
#include <time.h>
#include <algorithm>

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
  int from_top_height,
  int from_bottom_height,
  int top_steepness,
  int bottom_steepness
)
{
  if (!from_bottom_height) {
    bottom_terrain_height = ::rand() % (area_height - min_passage_height - 1) + 1;
  } else {
    int range;
    
    if (from_bottom_height - bottom_steepness < 1) {
      range = bottom_steepness + 1;
    } else {
      range = bottom_steepness * 2 + 1;
    }
  
    bottom_terrain_height = std::min<int>(
      ::rand() % (range) + std::max<int>(from_bottom_height - bottom_steepness, 1),
      area_height - from_top_height - min_passage_height - (from_top_height ? 0 : 1)
    );
  }

  int spaceLeft = area_height - bottom_terrain_height;

  if (!from_top_height) {
    top_terrain_height = ::rand() % (area_height - bottom_terrain_height - min_passage_height) + 1;
  } else {
    int range;
    
    if (from_top_height - top_steepness < 1) {
      range = top_steepness + 1;
    } else {
      range = top_steepness * 2 + 1;
    }
    
    top_terrain_height = std::min<int>(
      ::rand() % (range) + std::max<int>(from_top_height - top_steepness, 1),
      std::min<int>(
        area_height - from_bottom_height - min_passage_height,
        spaceLeft - min_passage_height
      )
    );
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
  int previousBottomTerrainHeight = from_bottom_height;
  int previousTopTerrainHeight = from_top_height;
  int bottomTerrainHeight, topTerrainHeight;
  
  // save area attr
  
  for (int i = 0; i < area->getWidthInBlocks(); ++i) {  
    calculateVerticalTerrainLine(
      bottomTerrainHeight,
      topTerrainHeight,
      area->getHeightInBlocks(),
      max_passage_height,
      min_passage_height,
      previousTopTerrainHeight,
      previousBottomTerrainHeight,
      top_steepness,
      bottom_steepness
    );
    
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
      if (j < topTerrainHeight || j >= area->getHeightInBlocks() - bottomTerrainHeight) {
        area->add(new Terrain(), i, j);
      }
    }
    
    previousBottomTerrainHeight = bottomTerrainHeight;
    previousTopTerrainHeight = topTerrainHeight;
  }
  
  // save area attr
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
  Area *adjancentTop    = NULL;
  Area *adjancentBottom = NULL;
  
  try {
    adjancentLeft = map.at(std::pair<int,int>(area_x - 1, area_y));
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentRight = map.at(std::pair<int,int>(area_x + 1, area_y));
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentTop = map.at(std::pair<int,int>(area_x, area_y - 1));
  } catch (std::out_of_range &e) {}
  
  try {
    adjancentBottom = map.at(std::pair<int,int>(area_x, area_y + 1));
  } catch (std::out_of_range &e) {}
  
  fillHorizontalPassage(area, area->getHeightInBlocks() - 2);
}
