#include <glade/debug/log.h>
#include <glade/math/Vector.h>
#include <strug/exception/StrugException.h>
#include <strug/Level.h>
#include <strug/blocks/Terrain.h>

const int Level::AREA_WIDTH_BLOCKS = 16;

Level::Level(int width_param, int height_param):
  width(0),
  height(0)
{
  if (width_param < 0) {
    throw StrugException("Width of the block level should be positive integer");
  }
  
  width  = width_param;
  height = height_param ? height_param : width_param;
  
  for (int y = 0; y < height; y++ ) {
    for (int x = 0; x < width; x++ ) {
      Vector2i blockCoordObject(x, y);
      levelData[blockCoordObject] = Level::Blocks();
      blockTypesData[blockCoordObject] = Level::BlockTypes();
    }
  }
  
  if (outOfBoundsCell.empty()) {
    outOfBoundsCell.push_back(new Terrain());
    outOfBoundsBlockTypes.clear();
    outOfBoundsBlockTypes.push_back(Block::TERRAIN);
  }
}

Level::~Level()
{
  BlocksMap::iterator cell = levelData.begin();
  
  while (cell != levelData.end()) {
    Level::Blocks::iterator block = cell->second.begin();
  
    while (block != cell->second.end()) {
      delete *block;
      ++block;
    }
    
    ++cell;
  }
  
  Level::Blocks::iterator block = outOfBoundsCell.begin();
  
  while (block != outOfBoundsCell.end()) {
    delete *block;
    ++block;
  }
}

Level::BlockTypes* Level::getObjectTypesAt(int x, int y)
{
  BlockTypesMap::iterator result = blockTypesData.find(Vector2i(x, y));
  
  if (result == blockTypesData.end()) {
    return &outOfBoundsBlockTypes;
  }
  
  return &result->second;
}

Level::Blocks* Level::getObjectsAt(int x, int y)
{
  BlocksMap::iterator result = levelData.find(Vector2i(x, y));
  
  if (result == levelData.end()) {
    return &outOfBoundsCell;
  }
  
  return &result->second;
}

Terrain* Level::getTerrainAt(int x, int y)
{
  Level::Blocks *blocks = getObjectsAt(x, y);

  if (blocks != NULL) {
    Level::Blocks::iterator block;
    
    for (block = blocks->begin(); block != blocks->end(); ++block) {
      if ((*block)->getType() == Block::TERRAIN) {
        return (Terrain*) (*block);
      }
    }
  }
  
  return NULL;
}

bool Level::isThereAnObjectOfType(Block::Type typeId, int blockX, int blockY)
{
  // FIXME use typedefs
  Level::BlockTypes *blockTypes = getObjectTypesAt(blockX, blockY);
  return std::find<Level::BlockTypes::iterator, Block::Type>(blockTypes->begin(), blockTypes->end(), typeId) != blockTypes->end();
}

void Level::add(Block *object, int blockX, int blockY)
{
  if (blockX >= width || blockY >= height || blockX < 0 || blockY < 0) {
    throw StrugException("Tried to access an object out of the level bounds");
  }
  
  if (object == NULL) {
    throw StrugException("Tried to add a NULL block to the level");
  }
  
  Level::Blocks *blocksInThisCell = &levelData.find(Vector2i(blockX, blockY))->second;
  Level::BlockTypes *blockTypesInThisCell = &blockTypesData.find(Vector2i(blockX, blockY))->second;
  
  Level::Blocks::iterator i = blocksInThisCell->begin();

  while (i != blocksInThisCell->end()) {
    try {
      if (coexistingBlockTypes.at((*i)->getType()) != object->getType()) {
        Level::BlockTypes::iterator ti =
          std::find<Level::BlockTypes::iterator, Block::Type>(
            blockTypesInThisCell->begin(), blockTypesInThisCell->end(), (*i)->getType()
          );
        
        blockTypesInThisCell->erase(ti);
        blocksInThisCell->erase(i);
      }
    } catch (std::out_of_range) {
      Level::BlockTypes::iterator ti =
        std::find<Level::BlockTypes::iterator, Block::Type>(
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

void Level::remove(int blockX, int blockY)
{
  if (blockX >= width || blockY >= height || blockX < 0 || blockY < 0) {
    throw StrugException("Tried to access an object out of the level bounds");
  }
  
  Level::Blocks *blocks = &levelData.find(Vector2i(blockX, blockY))->second;
  Level::Blocks::iterator block = blocks->begin();
  
  while (block != blocks->end()) {
    delete *block;
  }
  
  blocks->clear();  
  blockTypesData.find(Vector2i(blockX, blockY))->second.clear();
}