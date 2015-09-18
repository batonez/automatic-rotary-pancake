#pragma once

#include <glade/GladeObject.h>
#include <glade/State.h>
#include <glade/debug/log.h>
#include <glade/physics/RectangleCollisionShape.h>
#include <glade/physics/RectangleCollisionResolver.h>
#include <strug/generator/WorldGenerator.h>

class Context;
class Block;
class StrugController;
class Player;
class Area;

class Play: public State
{
  friend class PlayController;

  // FIXME Maybe it should be hash table?
  typedef std::map< std::pair<int, int>, Area* > AreaMap;
  
  private:
    static const float BASE_RUNNING_SPEED;

    float runningSpeed;
    float blockWidth, blockHeight;
    float screenScaleX, screenScaleY;
    Vector2i cameraMan;
    StrugController *controller;
    Player *player;
    WorldGenerator generator;
    AreaMap areaMap;
    AreaMap deletedAreaMap;
    RectangleCollisionShape staticColShape, kinematicColShape;
    RectangleCollisionResolver resolver;
    
    int
      prevPlayerBlockCoordX,
      prevPlayerBlockCoordY,
      prevPlayerAreaCoordX,
      prevPlayerAreaCoordY;

  public:
    Play();
    ~Play();
    
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
      
      float blockToWorldCoordXNotCentered(int blockX); //
      float blockToWorldCoordYNotCentered(int blockY); //
      void applyStartingRulesForMonolith(Block &block, int block_x, int block_y); //
      
      
      int getBlockCoordX(Block &object);
      int getBlockCoordY(Block &object);
      int areaCoordFromBlockCoord(int blockCoord);
      void applyStartingRulesForBlock(Block &block, int block_x, int block_y);
      void addArea(Context &context, int area_x, int area_y, MazeGenerator::CellType type);
      void addMoreAreas(Context &context, int area_x, int area_y);
      void removeFarAreas(Context &context, int area_x, int area_y);
};
