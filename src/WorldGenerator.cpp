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

static void fillVerticalLine()
{
  
}

static void fillHorizontalPassage(
  Area *area, int maxPassageHeight, int from_top_height = 0,
  int from_bottom_height = 0, int minPassageHeight = 1, int topSteepness = 2,
  int bottomSteepness = 2)
{
  int bottomTerrainHeight;
  int topTerrainHeight;
  int previousBottomTerrainHeight = from_bottom_height;
  int previousTopTerrainHeight = from_top_height;
  
  for (int i = 0; i < area->getWidthInBlocks(); ++i) {
    if (!previousBottomTerrainHeight) {
      bottomTerrainHeight = ::rand() % (area->getHeightInBlocks() - minPassageHeight - 1) + 1;
    } else {
      int range;
      
      if (previousBottomTerrainHeight - bottomSteepness < 1) {
        range = bottomSteepness + 1;
      } else {
        range = bottomSteepness * 2 + 1;
      }
    
      bottomTerrainHeight = std::min<int>(
        ::rand() % (range) + std::max<int>(previousBottomTerrainHeight - bottomSteepness, 1),
        area->getHeightInBlocks() - previousTopTerrainHeight - minPassageHeight - (previousTopTerrainHeight ? 0 : 1)
      );
    }
  
    int spaceLeft = area->getHeightInBlocks() - bottomTerrainHeight;
  
    if (!previousTopTerrainHeight) {
      topTerrainHeight = ::rand() % (area->getHeightInBlocks() - bottomTerrainHeight - minPassageHeight) + 1;
    } else {
      int range;
      
      if (previousTopTerrainHeight - topSteepness < 1) {
        range = topSteepness + 1;
      } else {
        range = topSteepness * 2 + 1;
      }
      
      topTerrainHeight = std::min<int>(
        ::rand() % (range) + std::max<int>(previousTopTerrainHeight - topSteepness, 1),
        std::min<int>(
          area->getHeightInBlocks() - previousBottomTerrainHeight - minPassageHeight,
          spaceLeft - minPassageHeight
        )
      );
    }
  
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
      if (j < topTerrainHeight || j >= area->getHeightInBlocks() - bottomTerrainHeight) {
        area->add(new Terrain(), i, j);
      }
    }
    
    previousBottomTerrainHeight = bottomTerrainHeight;
    previousTopTerrainHeight = topTerrainHeight;
  }
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
