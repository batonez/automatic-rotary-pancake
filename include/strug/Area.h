#pragma once

#include <vector>
#include <map>
#include <string>

#include <strug/blocks/Block.h>

namespace Strug
{
  class ResourceManager;
}

class Vector2i;
class Terrain;

class Area
{
  friend class Strug::ResourceManager;
  friend class WorldGenerator;
  
  public:
    typedef std::vector<Block::Type>      BlockTypes;
    typedef std::vector<Block*>           Blocks;
    typedef std::map<Vector2i,BlockTypes> BlockTypesMap;
    typedef std::map<Vector2i,Blocks>     BlocksMap;
    
    enum Type
    {
      AREA_BLOCKY,
      AREA_MONOLITH
    };
    
    static const int   AREA_WIDTH_BLOCKS;
    
    std::string       texturePackName;
    
  protected:
    int width, height;
  
    // These are contents that are considered for all cells outside Area bounds
    Blocks      outOfBoundsCell;
    BlockTypes  outOfBoundsBlockTypes;
    
  public:
    Area(int width, int height = 0);
    virtual ~Area();
    virtual Blocks*  getObjectsAt(int x, int y) = 0;
    virtual Terrain* getTerrainAt(int x, int y) = 0;
    virtual bool isThereAnObjectOfType(Block::Type typeId, int blockX, int blockY) = 0;
    // Acquires ownership of the Block
    virtual void add(Block *object, int blockX, int blockY) = 0;
    virtual void remove(int blockX, int blockY) = 0;
    virtual BlockTypes* getObjectTypesAt(int x, int y) = 0;
    virtual Block* getBlockByLabel(int label) = 0;
    virtual Type getType() = 0;
    
    virtual int    getWidthInBlocks()            { return width; }
    virtual int    getHeightInBlocks()           { return height; }
    virtual bool   isThereATerrain(int x, int y) { return getTerrainAt(x, y) != NULL; }
};
