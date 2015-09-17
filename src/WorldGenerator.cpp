#include <glade/debug/log.h>
#include <strug/WorldGenerator.h>
#include <strug/Level.h>
#include <strug/blocks/Terrain.h>

#include <stdlib.h>
#include <time.h>

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

static void fillHorizontalPassage(Area *area, int minPassageHeight, int maxPassageHeight)
{  
  for (int i = 0; i < area->getWidthInBlocks(); ++i) {
    int passageHeight = ::rand() % maxPassageHeight + minPassageHeight + 1;
    int halfPassageHeight = passageHeight / 2;
    
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
      if (j < halfPassageHeight || j > area->getHeightInBlocks() - halfPassageHeight) {
          area->add(new Terrain(), i, j);
      }
    }
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

void WorldGenerator::fillArea(Area *area)
{
  area->texturePackName = "cave";
  fillHorizontalPassage(area, 1, area->getHeightInBlocks() - 2);
}
