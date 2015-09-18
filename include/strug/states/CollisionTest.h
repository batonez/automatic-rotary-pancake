#pragma once

#include <glade/GladeObject.h>
#include <glade/State.h>
#include <glade/debug/log.h>
#include <glade/physics/RectangleCollisionShape.h>
#include <glade/physics/RectangleCollisionResolver.h>
#include <strug/Area.h>
#include <strug/generator/WorldGenerator.h>

class Context;
class Block;
class StrugController;
class Player;
class Terrain;

class CollisionTest: public State
{
  friend class MazeController;
  
  private:
    static const float BASE_RUNNING_SPEED;

    float runningSpeed;
    float blockWidth, blockHeight;
    float screenScaleX, screenScaleY;
    Vector2i cameraMan;
    StrugController *controller;
    Player *player;
    Terrain *terrain;
    RectangleCollisionShape staticColShape, kinematicColShape;
    RectangleCollisionResolver resolver;

  public:
    CollisionTest();
    ~CollisionTest();
    
    virtual void init(Context &context);
    virtual void shutdown(Context &context);
    virtual void applyRules(Context &context);
    
    private:
      float blockToWorldCoordX(int blockX)
      {
        return blockX * blockWidth - screenScaleX + blockWidth / 2;
      }
      
      float blockToWorldCoordY(int blockY)
      {
        return blockY * blockHeight - screenScaleY + blockHeight / 2;
      }

      int getBlockCoordX(Block &object);
      int getBlockCoordY(Block &object);
      int areaCoordFromBlockCoord(int blockCoord);
      void applyStartingRulesForBlock(Block &block, int block_x, int block_y);
};