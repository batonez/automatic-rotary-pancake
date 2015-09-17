#pragma once

#include <strug/blocks/Collectable.h>

class SpeedupShot: public Collectable
{
  public:
    SpeedupShot();
    void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    Block::Type getType() const { return Block::SPEEDUP_SHOT; }
    Collectable::Weapon getWeaponPowerUp() { return Collectable::SPEEDINGGUN; }
};
