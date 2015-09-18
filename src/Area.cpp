#include <strug/exception/StrugException.h>
#include <strug/Area.h>
#include <strug/blocks/Terrain.h>

const int Area::AREA_WIDTH_BLOCKS = 16;

Area::Area(int width_param, int height_param):
  width(0),
  height(0)
{
  if (width_param < 0) {
    throw StrugException("Width of the block Area should be positive integer");
  }
  
  width  = width_param;
  height = height_param ? height_param : width_param;
  
  if (outOfBoundsCell.empty()) {
    outOfBoundsCell.push_back(new Terrain());
    outOfBoundsBlockTypes.clear();
    outOfBoundsBlockTypes.push_back(Block::TERRAIN);
  }
}

Area::~Area()
{
  Area::Blocks::iterator block = outOfBoundsCell.begin();
  
  while (block != outOfBoundsCell.end()) {
    delete *block;
    ++block;
  }
}
