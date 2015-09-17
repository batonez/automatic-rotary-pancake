#pragma once

#include <strug/blocks/Block.h>

class Effect: public Block
{
  friend class EffectAnimationCallback;
  
  private:
    Drawable *view;
    Effect();
    
  public:
    static Effect INSTANCE;
    static void play(float x, float y, Callable *callback);
    static void play(float x, float y) { play(x, y, NULL); }
    
    ~Effect();
    void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    Block::Type getType() const { return Block::EFFECT; }
    bool isOverlapChecked() { return false; }
    bool isMoveable() { return false; }
    bool isUsable() { return false; }
};
