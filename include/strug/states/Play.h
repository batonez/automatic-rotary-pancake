#pragma once

#include <map>
#include <glade/GladeObject.h>
#include <glade/State.h>
#include <glade/math/Vector.h>
#include <strug/LevelInfo.h>
#include <strug/WorldGenerator.h>

class Context;
class Block;
class StrugController;
class Player;

class Play: public State
{
  friend class CharacterController;
  
  private:
    static const float BASE_RUNNING_SPEED;
    static const int   AREA_WIDTH_BLOCKS;
    
    float runningSpeed;
    float blockWidth, blockHeight;
    float screenScaleX, screenScaleY;
    WorldGenerator generator;
    Vector2i cameraMan;
    StrugController *controller;
    
    Player *player;
    
    // FIXME Should be hash table!!
    typedef std::map< std::pair<int, int>, Area* > AreaMap;
    AreaMap areaMap;

  public:
    Play();
    ~Play();
    
    virtual void init(Context &context);
    virtual void shutdown(Context &context);
    virtual void applyRules(Context &context);
    
    private:
      float blockToWorldCoordX(int blockX)
      {
        return ((float) blockX) * blockWidth - screenScaleX + blockWidth / 2;
      }
      
      float blockToWorldCoordY(int blockY)
      {
        return ((float) blockY) * blockHeight - screenScaleY + blockHeight / 2;
      }
      
      int getBlockCoordX(Block &object);
      int getBlockCoordY(Block &object);
      int Play::areaCoordFromBlockCoord(int blockCoord);
      
      void addArea(Context &context, int area_x, int area_y);
      void applyStartingRulesForBlock(Block &block, int block_x, int block_y);
};
