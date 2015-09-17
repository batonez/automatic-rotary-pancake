#pragma once

#include <strug/blocks/Terrain.h>

class Drawable;

class Mud: public Terrain
{
  private:
    Drawable *baseView, *topOverlay;
  
  public:
    Mud(Block::Properties *properties = NULL);
    virtual ~Mud();
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    virtual void updateView();
    virtual Block::Type getType() const { return Block::MUD; }
};
