#pragma once

#include <strug/blocks/Block.h>

class Exit: public Block
{
  public:
    static const int ANIMATION_ACTIVE = 0;
    static const int ANIMATION_TIME_ACTIVE = 800;
  
  private:
    bool active;
    Drawable *activeView, *inactiveView;
  
  public:
    Exit();
    virtual ~Exit();
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    
    virtual void updateView()
    {
      toggleView(*activeView, active);
      toggleView(*inactiveView, !active);
    }
    
    virtual void activate()
    {
      active = true;
      updateView();
    }
    
    virtual void deactivate()
    {
      active = false;
      updateView();
    }
    
    virtual bool isActive() { return active; }
    virtual Block::Type getType() const { return Block::EXIT; }
    virtual bool isOverlapChecked() { return true; }
    virtual bool isMoveable() { return false; }
    virtual bool isUsable() { return false; }
};
