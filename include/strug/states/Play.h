#pragma once

#include <glade/GladeObject.h>
#include <glade/State.h>
#include <strug/LevelInfo.h>
#include <strug/WorldGenerator.h>

class Context;
class Block;

class Play: public State
{
  private:
    static const float BASE_RUNNING_SPEED;
    static const int   AREA_WIDTH_BLOCKS;
    
    float runningSpeed;
    float blockWidth, blockHeight;
    float levelScaleX, levelScaleY;
    GladeObject background;
    Drawable *backgroundView;
    LevelInfo levelInfo;
    WorldGenerator generator;
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
