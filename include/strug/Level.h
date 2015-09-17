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

// TODO rename to Area
class Level
{
  friend class Strug::ResourceManager;
  friend class WorldGenerator;
  
  public:
    typedef std::vector<Block::Type>      BlockTypes;
    typedef std::vector<Block*>           Blocks;
    typedef std::map<Vector2i,BlockTypes> BlockTypesMap;
    typedef std::map<Vector2i,Blocks>     BlocksMap;
    typedef std::map<std::string, int>    IntGenerationInfo;
    
    static const int   AREA_WIDTH_BLOCKS;
    
    BlocksMap         levelData;
    BlockTypesMap     blockTypesData;
    std::string       texturePackName;
    IntGenerationInfo intAttributes;
  
  private:
    int width, height;
    std::map<int,Block*>      labeledBlocks; // may point to a deleted object. Always check levelData first
    std::map<int,Block::Type> coexistingBlockTypes; // (base block) -> (overlay block)
  
    // These are contents that are considered for all cells outside level bounds
    Blocks      outOfBoundsCell;
    BlockTypes  outOfBoundsBlockTypes;
    
  public:
    Level(int width, int height = 0);
    ~Level();
    Blocks*  getObjectsAt(int x, int y);
    Terrain* getTerrainAt(int x, int y);
    bool isThereAnObjectOfType(Block::Type typeId, int blockX, int blockY);
    // Acquires ownership of the Block
    void add(Block *object, int blockX, int blockY);
    void remove(int blockX, int blockY);
    BlockTypes* getObjectTypesAt(int x, int y);
    
    int    getWidthInBlocks()            { return width; }
    int    getHeightInBlocks()           { return height; }
    bool   isThereATerrain(int x, int y) { return getTerrainAt(x, y) != NULL; }
    Block* getBlockByLabel(int label)    { return labeledBlocks.at(label); }
};

typedef Level Area;

// FIXME Maybe it should be hash table?
typedef std::map< std::pair<int, int>, Area* > AreaMap;
