#include <glade/render/Drawable.h>
#include <glade/render/meshes/Rectangle.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Npc.h>

extern Strug::ResourceManager *game_resource_manager;

std::map<Character::Animation, int> Npc::animationRowNumbers;
//static Map<SoundType,Sound> soundMap = new EnumMap<SoundType,Sound>(SoundType.class);
  
Npc::Npc():
  Character()
{
  setName("Enemy stalker");
  
  if (animationRowNumbers.empty()) {
    animationRowNumbers[Character::IDLE] = 0;
    animationRowNumbers[Character::RUNNING] = 1;
    animationRowNumbers[Character::MOVING_UP_ON_LADDER] = 2;
    animationRowNumbers[Character::MOVING_DOWN_ON_LADDER] = 3;
    animationRowNumbers[Character::LEAP] = 4;
    animationRowNumbers[Character::FALLING] = 5;
    animationRowNumbers[Character::HANGING_ON_PIPE] = 6;
    animationRowNumbers[Character::MOVING_ON_PIPE] = 7;
    
    animationRowNumbers[Character::BURNING] = 1;
    animationRowNumbers[Character::ASH] = 1;
    animationRowNumbers[Character::IMPALE] = 1;
    animationRowNumbers[Character::TELEPORT] = 1;
  }
}

void Npc::initialize(const std::string &texture_pack_name, float block_width, float block_height)
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
    view->setTexture(game_resource_manager->getTexture(texture_pack_name, "robot.png", "frame_size_64.csv"));
    /*
    Sound landingSound = SoundManager.getSound(texture_pack_name, SoundManager.LANDING_GROUP_ID);
    Sound laserSound = SoundManager.getSound(texture_pack_name, SoundManager.LASER_GROUP_ID);
    Sound fallingSound = SoundManager.getSound(texture_pack_name, SoundManager.ROBOT_FALLING_GROUP_ID);
    soundMap.put(SoundType.LANDING, landingSound);
    soundMap.put(SoundType.LASER, laserSound);
    soundMap.put(SoundType.FALLING, fallingSound);
    addSound(fallingSound);
    addSound(landingSound);
    addSound(laserSound);
    */
  }
}
