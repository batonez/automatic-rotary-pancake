#include <glade/render/Drawable.h>
#include <glade/render/TextureTransform.h>
#include <strug/blocks/Character.h>

class WeaponCallback: public TextureTransform::AnimationCallback
{
  public:
    WeaponCallback(Callable *callback):
      TextureTransform::AnimationCallback(callback)
    {}
    
    virtual int getTriggeringFrame() { return 5; }
};

class WeaponSoundCallback: public TextureTransform::AnimationCallback
{
  public:
    WeaponSoundCallback():
      TextureTransform::AnimationCallback()
    {}
    
    virtual void call() { log("WeaponSound"); /* getSound(Character.SoundType.LASER).play(); */}
    virtual int getTriggeringFrame() { return 2; }
};

class UnlockCharacterAtTheEndOfAnimation: public TextureTransform::AnimationCallback
{
  private:
    Character &character;
  public:
    UnlockCharacterAtTheEndOfAnimation(Character &character_param):
      TextureTransform::AnimationCallback(),
      character(character_param)
    {}
    
    virtual void call() { log("UnlockAtTHeEnd"); character.locked = false; }
    virtual int getTriggeringFrame() { return 7; }
};

class AfterOneAnimationCycle: public TextureTransform::AnimationCallback
{
  public:
    AfterOneAnimationCycle(Callable *callback):
      TextureTransform::AnimationCallback(callback)
    {}
    
    virtual int getTriggeringFrame() { return 7; }
};

std::map<Character::Animation,float> Character::animationTimes;

Character::Character():
  Block(),
  atLadder(false),
  onLadder(false),
  atPipe(false),
  isFalling(false),
  onTheGround(false),
  isClimbingLadder(false),
  alreadyFellDownForOneBlock(false),
  justLeaped(false),
  wantsToGoRight(false),
  wantsToGoLeft(false),
  wantsToGoDown(false),
  wantsToGoUp(false),
  locked(false),
  dead(false),
  goldCollected(0),
  view(NULL)
{
  if (animationTimes.empty()) {
    animationTimes[IDLE] = 2200.0f;
    animationTimes[RUNNING] = 700.0f;
    animationTimes[MOVING_UP_ON_LADDER] = 800.0f;
    animationTimes[MOVING_DOWN_ON_LADDER] = 800.0f;
    animationTimes[MOVING_ON_PIPE] = 700.0f;
    animationTimes[HANGING_ON_PIPE] = 2000.0f;
    animationTimes[BURNING] = 900.0f;
    animationTimes[FALLING] = 500.0f;
    animationTimes[ASH] = 800.0f;
    animationTimes[IMPALE] = 400.0f;
    animationTimes[TELEPORT] = 800.0f;
    animationTimes[LEAP] = 600.0f;
  }
}

Character::~Character()
{
  if (view) {
    delete view; 
    view = NULL;
  }
}

void Character::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    Block::initialize(texture_pack_name, block_width, block_height);
    log("SETTING CHARACTER PHYS BODY");
    setPhysicBody(physicBody);
 }
}

void Character::resetIntentions()
{
  wantsToGoRight = false;
  wantsToGoLeft  = false;
  wantsToGoDown  = false;
  wantsToGoUp    = false;
}

void Character::faceLeft()
{
  view->getTextureTransform()->flipTextureHorizontally(true);
}

void Character::faceRight()
{
  view->getTextureTransform()->flipTextureHorizontally(false);
}

bool Character::isFacingRight()
{
  return !view->getTextureTransform()->isFlippedHorizontally();
}

void Character::hangOnLadder()
{
  if (!locked) {
    if (!justLeaped) {
      climbVertically(false);
      view->getTextureTransform()->freezeAnimation(true);
    }
  }
}

void Character::hangOnPipe()
{
  if (!locked) {
    view->getTextureTransform()->animate(
      getAnimationRow(HANGING_ON_PIPE),
      animationTimes[HANGING_ON_PIPE],
      true
    );
  }
}

void Character::stand()
{
  if (!locked) {
    view->getTextureTransform()->animate(
      getAnimationRow(IDLE),
      animationTimes[IDLE],
      true
    );
  }
}

void Character::climbHorizontally()
{
  if (!locked) {
    view->getTextureTransform()->animate(
      getAnimationRow(MOVING_ON_PIPE),
      animationTimes[MOVING_ON_PIPE],
      true
    );
  }
}

void Character::climbVertically(bool up)
{
  if (!locked) {
    justLeaped = false;
    
    view->getTextureTransform()->animate(
      getAnimationRow(up ? MOVING_UP_ON_LADDER : MOVING_DOWN_ON_LADDER),
      animationTimes[up ? MOVING_UP_ON_LADDER : MOVING_DOWN_ON_LADDER],
      true
    );
  }
}

void Character::run()
{
  if (!locked) {
    view->getTextureTransform()->animate(
      getAnimationRow(RUNNING),
      animationTimes[RUNNING],
      true
    );
  }
}

void Character::fly()
{
  view->getTextureTransform()->animate(
    getAnimationRow(FALLING),
    animationTimes[FALLING],
    true
  );
}

void Character::fireWeapon(Callable *callback)
{
  locked = true;
  view->getTextureTransform()->animate(
    getAnimationRow(BURNING),
    animationTimes[BURNING],
    false
  );
  
  view->getTextureTransform()->addAnimationCallback(new WeaponCallback(callback));
  view->getTextureTransform()->addAnimationCallback(new WeaponSoundCallback());
  view->getTextureTransform()->addAnimationCallback(new UnlockCharacterAtTheEndOfAnimation(*this));
}

void Character::leap(bool useLeapingAnimation)
{
  if (!locked) {
    locked = true;
    view->getTextureTransform()->animate(
      getAnimationRow(useLeapingAnimation ? LEAP : RUNNING),
      animationTimes[useLeapingAnimation ? LEAP : RUNNING],
      false,
      true
    );
    
    justLeaped = useLeapingAnimation;
    
    view->getTextureTransform()->addAnimationCallback(new UnlockCharacterAtTheEndOfAnimation(*this));
  }
}

void Character::impale()
{
  locked = true;
  view->getTextureTransform()->animate(
    getAnimationRow(IMPALE),
    animationTimes[IMPALE],
    false
  );
  
  view->getTextureTransform()->addAnimationCallback(new UnlockCharacterAtTheEndOfAnimation(*this));
  // getSound(SoundType.IMPALE).play();
}

void Character::ash()
{
  locked = true;
  view->getTextureTransform()->animate(
    getAnimationRow(ASH),
    animationTimes[ASH],
    false
  );
  
  view->getTextureTransform()->addAnimationCallback(new UnlockCharacterAtTheEndOfAnimation(*this));
}

void Character::teleport(Callable *callback)
{
  locked = true;
  
  view->getTextureTransform()->animate(
    getAnimationRow(TELEPORT),
    animationTimes[TELEPORT],
    false
  );
  
  view->getTextureTransform()->addAnimationCallback(new AfterOneAnimationCycle(callback));
}

void Character::land()
{
  if (!isDead()) {
    // getSound(Character.SoundType.LANDING).play();
  }
}
