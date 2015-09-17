#pragma once

#include <string>
#include <strug/blocks/Block.h>

class Level;

class Terrain: public Block
{
  private:
    Drawable *baseView, *topOverlay;
    
  public:
    Terrain();
    ~Terrain();
    
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    virtual void updateView();
    
    bool looksLike(Block *block)
    {
      return Block::looksLike(*block);
    }
    
    virtual Block::Type getType() const { return Block::TERRAIN; }
    
    virtual bool isOverlapChecked() { return false; }
    virtual bool isMoveable() { return false; }
    virtual bool isUsable() { return false; }
};
