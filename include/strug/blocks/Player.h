#pragma once

#include <map>

#include <strug/blocks/Character.h>

class Player: public Character
{
  protected:
    static std::map<Character::Animation,int> animationRowNumbers;
    //static std::map<SoundType,Sound> soundMap = new EnumMap<SoundType,Sound>(SoundType.class);
  
  public:
    Player();
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);
    virtual Block::Type getType() const { return Block::PLAYER; }

/*
    @Override
    Sound getSound(SoundType soundId)
    {
      return soundMap.get(soundId);
    }*/
    
  protected:
    int getAnimationRow(Character::Animation animation)
    {
      return animationRowNumbers[animation];
    }
};
