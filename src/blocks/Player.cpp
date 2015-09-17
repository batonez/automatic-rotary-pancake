#include <glade/render/Drawable.h>
#include <glade/render/meshes/Rectangle.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Player.h>

extern Strug::ResourceManager *game_resource_manager;

std::map<Character::Animation,int> Player::animationRowNumbers;
//std::map<SoundType,Sound> Player::soundMap;
  
Player::Player():
  Character()
{
  setName("Player");
  
  if (animationRowNumbers.empty())
  {
    animationRowNumbers[Character::IDLE] = 0;
    animationRowNumbers[Character::RUNNING] = 1;
    animationRowNumbers[Character::MOVING_UP_ON_LADDER] = 2;
    animationRowNumbers[Character::MOVING_DOWN_ON_LADDER] = 2;
    animationRowNumbers[Character::MOVING_ON_PIPE] = 3;
    animationRowNumbers[Character::HANGING_ON_PIPE] = 4;
    animationRowNumbers[Character::BURNING] = 5;
    animationRowNumbers[Character::FALLING] = 6;
    animationRowNumbers[Character::ASH] = 7;
    animationRowNumbers[Character::IMPALE] = 8;
    animationRowNumbers[Character::TELEPORT] = 9;
    animationRowNumbers[Character::LEAP] = 10;
  }
}
    
void Player::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized) {
    Character::initialize(texture_pack_name, block_width, block_height);
    
    std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram(
        "texcoord_frames.vertex.glsl",
        "textured.fragment.glsl"
      );
        
    view = new Drawable(Rectangle::INSTANCE, program);
    addDrawable(view);
    view->setTexture(game_resource_manager->getTexture(texture_pack_name, "player.png", "frame_size_64.csv"));
    
    //Sound landingSound = SoundManager.getSound(texture_pack_name, SoundManager.LANDING_GROUP_ID);
    //Sound laserSound = SoundManager.getSound(texture_pack_name, SoundManager.LASER_GROUP_ID);
    //Sound impaleSound = SoundManager.getSound(texture_pack_name, SoundManager.IMPALE_GROUP_ID);
    //soundMap.put(SoundType.LANDING, landingSound);
    //soundMap.put(SoundType.LASER, laserSound);
    //soundMap.put(SoundType.IMPALE, impaleSound);
    //addSound(landingSound);
    //addSound(laserSound);
    //addSound(impaleSound);
  }
}
