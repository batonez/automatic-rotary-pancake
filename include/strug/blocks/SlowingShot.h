#pragma once

#include <strug/blocks/Collectable.h>

class SlowingShot: public Collectable
{
  public:
    SlowingShot();
    void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    Block::Type getType() const { return Block::SLOWING_SHOT; }
    Collectable::Weapon getWeaponPowerUp() { return Collectable::SLOWINGGUN; }
};
