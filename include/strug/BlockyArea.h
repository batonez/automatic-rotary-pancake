#pragma once

#include <vector>
#include <map>
#include <string>

#include <strug/blocks/Block.h>
#include <strug/Area.h>

namespace Strug
{
  class ResourceManager;
}

class Vector2i;
class Terrain;

class BlockyArea: public Area
{
  friend class Strug::ResourceManager;
  friend class WorldGenerator;
  
  private:
    BlocksMap         areaData;
    BlockTypesMap     blockTypesData;
    
    std::map<int,Block*>      labeledBlocks; // may point to a deleted object. Always check AreaData first
    std::map<int,Block::Type> coexistingBlockTypes; // (base block) -> (overlay block)
      
  public:
    BlockyArea(int width, int height = 0);
    virtual ~BlockyArea();
    virtual Blocks*  getObjectsAt(int x, int y);
    virtual Terrain* getTerrainAt(int x, int y);
    virtual bool isThereAnObjectOfType(Block::Type typeId, int blockX, int blockY);
    // Acquires ownership of the Block
    virtual void add(Block *object, int blockX, int blockY);
    virtual void remove(int blockX, int blockY);
    virtual BlockTypes* getObjectTypesAt(int x, int y);
    
    virtual Block* getBlockByLabel(int label)    { return labeledBlocks.at(label); }
};
