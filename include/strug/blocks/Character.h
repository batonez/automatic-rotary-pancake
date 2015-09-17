#pragma once

#include <map>

#include <strug/blocks/Block.h>

class Callable;

class Character: public Block
{
  friend class UnlockCharacterAtTheEndOfAnimation;
  
  public:
    enum SoundType
    {
      SOUND_LANDING,
      SOUND_LASER,
      SOUND_IMPALE,
      SOUND_FALLING
    };
    
    enum Animation
    {
      IDLE,
      RUNNING,
      MOVING_UP_ON_LADDER,
      MOVING_DOWN_ON_LADDER,
      MOVING_ON_PIPE,
      HANGING_ON_PIPE,
      BURNING,
      FALLING,
      ASH,
      IMPALE,
      TELEPORT,
      LEAP
    };
    
  protected:
    static std::map<Animation,float> animationTimes;
    Drawable *view;
    
  public:
    bool atLadder;
    bool onLadder;
    bool atPipe;
    bool isFalling;
    bool onTheGround;
    bool isClimbingLadder;
    bool alreadyFellDownForOneBlock;
    bool justLeaped;
    
    bool wantsToGoRight;
    bool wantsToGoLeft;
    bool wantsToGoDown;
    bool wantsToGoUp;
    
    bool locked;
    int  goldCollected;
  
  private:
    bool dead;

  protected:
    virtual int getAnimationRow(Animation animation) = 0;
    
  public:
    //virtual Sound getSound(SoundType soundId) = 0;
    
    Character();
    virtual ~Character();
    void resetIntentions();
    void faceLeft();
    void faceRight();
    bool isFacingRight();
    void hangOnLadder();
    void hangOnPipe();
    void stand();
    void climbHorizontally();
    void climbVertically(bool up);
    void run();
    void fly();
    void leap(bool useLeapingAnimation);
    void impale();
    void ash();
    void fireWeapon(Callable *callback);
    void teleport(Callable *callback);
    void land();
    void startFalling() {}
    
    void setDead(bool dead_param)
    {
      dead = dead_param;
      locked = dead_param;
    }

    bool isDead() { return dead; }
    void setVisible(bool visible) { toggleView(*view, visible); }
    bool isMoveable() { return true; }
    bool isOverlapChecked() { return true; }
    bool isUsable() { return false; }
};
