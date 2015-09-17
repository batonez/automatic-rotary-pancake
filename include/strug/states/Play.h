#pragma once

#include <glade/GladeObject.h>
#include <glade/State.h>
#include <strug/LevelInfo.h>

class Context;
class Block;

class Play: public State
{
  private:
    static const float
      BASE_FALLING_SPEED,
      BASE_RUNNING_SPEED,
      BASE_LADDER_UP_SPEED,
      BASE_LADDER_DOWN_SPEED,
      BASE_TRAVELATOR_SPEED,
      CUBE_ROTATION_SPEED,
      SWAMP_SPEED_FACTOR,
      NPC_SPEED_FACTOR;
      
    float
      fallingSpeed,
      runningSpeed,
      ladderUpSpeed,
      ladderDownSpeed,
      travelatorSpeed;
      
    float blockWidth, blockHeight;
    float levelScaleX, levelScaleY;
    GladeObject background;
    Drawable *backgroundView;
    LevelInfo levelInfo;
    std::shared_ptr<Level> level;

  public:
    Play(const LevelInfo &level_info);
    ~Play();
    
    virtual void init(Context &context);
    virtual void shutdown() {}
    virtual void applyRules(Context &context) {}
    
    void applyStartingRulesForBlock(const Block &block, int block_x, int block_y);
    
    private:
      float blockToWorldCoordX(int blockX)
      {
        return blockX * blockWidth - levelScaleX + blockWidth / 2;
      }
      
      float blockToWorldCoordY(int blockY)
      {
        return blockY * blockHeight - levelScaleY + blockHeight / 2;
      }
};
