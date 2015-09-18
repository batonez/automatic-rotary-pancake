#include <glade/debug/log.h>
#include <glade/math/Vector.h>
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
  
  for (int y = 0; y < height; y++ ) {
    for (int x = 0; x < width; x++ ) {
      Vector2i blockCoordObject(x, y);
      AreaData[blockCoordObject] = Area::Blocks();
      blockTypesData[blockCoordObject] = Area::BlockTypes();
    }
  }
  
  if (outOfBoundsCell.empty()) {
    outOfBoundsCell.push_back(new Terrain());
    outOfBoundsBlockTypes.clear();
    outOfBoundsBlockTypes.push_back(Block::TERRAIN);
  }
}

Area::~Area()
{
  BlocksMap::iterator cell = AreaData.begin();
  
  while (cell != AreaData.end()) {
    Area::Blocks::iterator block = cell->second.begin();
  
    while (block != cell->second.end()) {
      delete *block;
      ++block;
    }
    
    ++cell;
  }
  
  Area::Blocks::iterator block = outOfBoundsCell.begin();
  
  while (block != outOfBoundsCell.end()) {
    delete *block;
    ++block;
  }
}

Area::BlockTypes* Area::getObjectTypesAt(int x, int y)
{
  BlockTypesMap::iterator result = blockTypesData.find(Vector2i(x, y));
  
  if (result == blockTypesData.end()) {
    return &outOfBoundsBlockTypes;
  }
  
  return &result->second;
}

Area::Blocks* Area::getObjectsAt(int x, int y)
{
  BlocksMap::iterator result = AreaData.find(Vector2i(x, y));
  
  if (result == AreaData.end()) {
    return &outOfBoundsCell;
  }
  
  return &result->second;
}

Terrain* Area::getTerrainAt(int x, int y)
{
  Area::Blocks *blocks = getObjectsAt(x, y);

  if (blocks != NULL) {
    Area::Blocks::iterator block;
    
    for (block = blocks->begin(); block != blocks->end(); ++block) {
      if ((*block)->getType() == Block::TERRAIN) {
        return (Terrain*) (*block);
      }
    }
  }
  
  return NULL;
}

bool Area::isThereAnObjectOfType(Block::Type typeId, int blockX, int blockY)
{
  // FIXME use typedefs
  Area::BlockTypes *blockTypes = getObjectTypesAt(blockX, blockY);
  return std::find<Area::BlockTypes::iterator, Block::Type>(blockTypes->begin(), blockTypes->end(), typeId) != blockTypes->end();
}

void Area::add(Block *object, int blockX, int blockY)
{
  if (blockX >= width || blockY >= height || blockX < 0 || blockY < 0) {
    throw StrugException("Tried to access an object out of the Area bounds");
  }
  
  if (object == NULL) {
    throw StrugException("Tried to add a NULL block to the Area");
  }
  
  Area::Blocks *blocksInThisCell = &AreaData.find(Vector2i(blockX, blockY))->second;
  Area::BlockTypes *blockTypesInThisCell = &blockTypesData.find(Vector2i(blockX, blockY))->second;
  
  Area::Blocks::iterator i = blocksInThisCell->begin();

  while (i != blocksInThisCell->end()) {
    try {
      if (coexistingBlockTypes.at((*i)->getType()) != object->getType()) {
        Area::BlockTypes::iterator ti =
          std::find<Area::BlockTypes::iterator, Block::Type>(
            blockTypesInThisCell->begin(), blockTypesInThisCell->end(), (*i)->getType()
          );
        
        blockTypesInThisCell->erase(ti);
        blocksInThisCell->erase(i);
      }
    } catch (std::out_of_range) {
      Area::BlockTypes::iterator ti =
        std::find<Area::BlockTypes::iterator, Block::Type>(
          blockTypesInThisCell->begin(), blockTypesInThisCell->end(), (*i)->getType()
        );
      blockTypesInThisCell->erase(ti);
      blocksInThisCell->erase(i);
    }
            
    ++i;
  }
  
  blocksInThisCell->push_back(object);
  blockTypesInThisCell->push_back(object->getType());
  i = blocksInThisCell->end() - 1;
  
  if (object->getLabel() != NULL) {
    labeledBlocks[object->getLabel()] = *i;
  }
}

void Area::remove(int blockX, int blockY)
{
  if (blockX >= width || blockY >= height || blockX < 0 || blockY < 0) {
    throw StrugException("Tried to access an object out of the Area bounds");
  }
  
  Area::Blocks *blocks = &AreaData.find(Vector2i(blockX, blockY))->second;
  Area::Blocks::iterator block = blocks->begin();
  
  while (block != blocks->end()) {
    delete *block;
  }
  
  blocks->clear();  
  blockTypesData.find(Vector2i(blockX, blockY))->second.clear();
}