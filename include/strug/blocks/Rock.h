#pragma once

#include <strug/blocks/Terrain.h>

extern Strug::ResourceManager *game_resource_manager;

class Rock: public Terrain
{
  private:
    Drawable *baseView, *rightEdgeOverlay, *bothEdgesOverlay, *noEdgesOverlay;
  
  public:
    Rock(Block::Properties *properties = NULL);
    virtual ~Rock();
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    virtual void updateView();
    virtual Block::Type getType() const { return Block::ROCK; }
    virtual bool isOverlapChecked() { return false; }
    virtual bool isMoveable() { return false; }
};
