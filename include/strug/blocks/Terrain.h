#pragma once

#include <string>
#include <strug/blocks/Block.h>

class Level;

class Terrain: public Block
{
  private:
    Drawable *baseView, *topOverlay;
    
  public:
    Terrain(Block::Properties *properties = NULL);
    ~Terrain();
    
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    virtual void updateView();
    
    bool looksLike(Block *block)
    {
      return Block::looksLike(*block);
    }
    
    virtual std::string toString()
    {
      std::string result = Block::toString();
      
      result += slowing ? " slowing=1" : "";
      result += propulsing ? " propulsing=1" : "";
      result += travelator ? " travelator=1" : "";
      result += travelator && leftToRight ? " leftToRight=1" : "";
      result += pedal ? " pedal=1" : "";
      result += controlledObjectLabel > 0 ? " controlledObject=" + controlledObjectLabel : "";
      
      return result;
    }
    
    virtual Block::Type getType() const { return Block::MUD; }
};
