#pragma once

#include <strug/blocks/Collectable.h>

class Gold: public Collectable
{
  public:
    Gold();
    void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    Block::Type getType() const { return Block::GOLD; }
};
