#pragma once

#include <strug/blocks/Block.h>

class Collectable: public Block
{
  public:
    enum Weapon
    {
      NONE, BURNER, SLOWINGGUN, SPEEDINGGUN
    };
  
  protected:
    Drawable *view;
  
  public:
    static Transform::SharedVector cubeRotation;
    
    Collectable();
    virtual ~Collectable();
    virtual Weapon getWeaponPowerUp();
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    virtual bool isOverlapChecked() { return true; }
    virtual bool isMoveable() { return false; } 
    virtual bool isUsable() { return false; }
    
    void disappear()
    {
      toggleCollisionShape(false);
      toggleView(*view, false);
    }
};
