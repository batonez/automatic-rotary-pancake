#include <glade/debug/log.h>
#include <glade/math/Vector.h>
#include <strug/exception/StrugException.h>
#include <strug/BlockyArea.h>
#include <strug/blocks/Terrain.h>

BlockyArea::BlockyArea(int width_param, int height_param):
  Area(width_param, height_param)
{
  for (int y = 0; y < height; y++ ) {
    for (int x = 0; x < width; x++ ) {
      Vector2i blockCoordObject(x, y);
      areaData[blockCoordObject] = BlockyArea::Blocks();
      blockTypesData[blockCoordObject] = BlockyArea::BlockTypes();
    }
  }
}

BlockyArea::~BlockyArea()
{
  BlocksMap::iterator cell = areaData.begin();
  
  while (cell != areaData.end()) {
    BlockyArea::Blocks::iterator block = cell->second.begin();
  
    while (block != cell->second.end()) {
      delete *block;
      ++block;
    }
    
    ++cell;
  }
}

BlockyArea::BlockTypes* BlockyArea::getObjectTypesAt(int x, int y)
{
  BlockTypesMap::iterator result = blockTypesData.find(Vector2i(x, y));
  
  if (result == blockTypesData.end()) {
    return &outOfBoundsBlockTypes;
  }
  
  return &result->second;
}

BlockyArea::Blocks* BlockyArea::getObjectsAt(int x, int y)
{
  BlocksMap::iterator result = areaData.find(Vector2i(x, y));
  
  if (result == areaData.end()) {
    return &outOfBoundsCell;
  }
  
  return &result->second;
}

Terrain* BlockyArea::getTerrainAt(int x, int y)
{
  BlockyArea::Blocks *blocks = getObjectsAt(x, y);

  if (blocks != NULL) {
    BlockyArea::Blocks::iterator block;
    
    for (block = blocks->begin(); block != blocks->end(); ++block) {
      if ((*block)->getType() == Block::TERRAIN) {
        return (Terrain*) (*block);
      }
    }
  }
  
  return NULL;
}

bool BlockyArea::isThereAnObjectOfType(Block::Type typeId, int blockX, int blockY)
{
  // FIXME use typedefs
  BlockyArea::BlockTypes *blockTypes = getObjectTypesAt(blockX, blockY);
  return std::find<BlockyArea::BlockTypes::iterator, Block::Type>(blockTypes->begin(), blockTypes->end(), typeId) != blockTypes->end();
}

void BlockyArea::add(Block *object, int blockX, int blockY)
{
  if (blockX >= width || blockY >= height || blockX < 0 || blockY < 0) {
    throw StrugException("Tried to access an object out of the Area bounds");
  }
  
  if (object == NULL) {
    throw StrugException("Tried to add a NULL block to the Area");
  }
  
  BlockyArea::Blocks *blocksInThisCell = &areaData.find(Vector2i(blockX, blockY))->second;
  BlockyArea::BlockTypes *blockTypesInThisCell = &blockTypesData.find(Vector2i(blockX, blockY))->second;
  
  BlockyArea::Blocks::iterator i = blocksInThisCell->begin();

  while (i != blocksInThisCell->end()) {
    try {
      if (coexistingBlockTypes.at((*i)->getType()) != object->getType()) {
        BlockyArea::BlockTypes::iterator ti =
          std::find<BlockyArea::BlockTypes::iterator, Block::Type>(
            blockTypesInThisCell->begin(), blockTypesInThisCell->end(), (*i)->getType()
          );
        
        blockTypesInThisCell->erase(ti);
        blocksInThisCell->erase(i);
      }
    } catch (std::out_of_range) {
      BlockyArea::BlockTypes::iterator ti =
        std::find<BlockyArea::BlockTypes::iterator, Block::Type>(
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
  
  if (object->getLabel() != 0) {
    labeledBlocks[object->getLabel()] = *i;
  }
}

void BlockyArea::remove(int blockX, int blockY)
{
  if (blockX >= width || blockY >= height || blockX < 0 || blockY < 0) {
    throw StrugException("Tried to access an object out of the Area bounds");
  }
  
  BlockyArea::Blocks *blocks = &areaData.find(Vector2i(blockX, blockY))->second;
  BlockyArea::Blocks::iterator block = blocks->begin();
  
  while (block != blocks->end()) {
    delete *block;
  }
  
  blocks->clear();  
  blockTypesData.find(Vector2i(blockX, blockY))->second.clear();
}