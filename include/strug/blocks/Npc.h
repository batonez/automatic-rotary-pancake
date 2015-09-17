#pragma once

#include <map>
#include <strug/blocks/Character.h>

class Npc: public Character
{
  protected:
    static std::map<Character::Animation, int> animationRowNumbers;
    //static Map<SoundType,Sound> soundMap = new EnumMap<SoundType,Sound>(SoundType.class);
  
  public:
    Npc();
    virtual void initialize(const std::string &texture_pack_name, float block_width, float block_height);

    Block::Type getType() const { return Block::NPC; }
    
    //void startFalling()
    //{
    //  getSound(SoundType.FALLING).play();
    //}
    
    //Sound getSound(SoundType soundId)
    //{
    //  return soundMap.get(soundId);
    //}

  protected:
    int getAnimationRow(Character::Animation animation)
    {
      return animationRowNumbers[animation];
    }
};
