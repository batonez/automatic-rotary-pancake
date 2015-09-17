//#include <glade/physics/CollisionEventListener.h>
//#include <glade/math/Graph.h>
#include <glade/Context.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Mud.h>
#include <strug/blocks/Rock.h>
#include <strug/blocks/Ladder.h>
#include <strug/blocks/Pipe.h>
#include <strug/blocks/Spikes.h>
#include <strug/blocks/Gold.h>
#include <strug/blocks/Exit.h>
#include <strug/blocks/SlowingShot.h>
#include <strug/blocks/SpeedupShot.h>
#include <strug/blocks/Effect.h>
#include <strug/blocks/Player.h>
#include <strug/blocks/Npc.h>

#include <strug/states/Play.h>

extern Strug::ResourceManager *game_resource_manager;

const float Play::BASE_FALLING_SPEED     = 0.008f;
const float Play::BASE_RUNNING_SPEED     = 0.004f;
const float Play::BASE_LADDER_UP_SPEED   = 0.002f;
const float Play::BASE_LADDER_DOWN_SPEED = 0.003f;
const float Play::BASE_TRAVELATOR_SPEED  = 0.002f;
const float Play::CUBE_ROTATION_SPEED    = 0.1f;
const float Play::SWAMP_SPEED_FACTOR     = 0.5f;
const float Play::NPC_SPEED_FACTOR       = 0.7f;

Play::Play(const LevelInfo &level_info):
  State(),
  levelScaleX(0),
  levelScaleY(0),
  backgroundView(NULL),
  levelInfo(level_info)
{}

Play::~Play()
{
  if (backgroundView != NULL) {
    delete backgroundView;
    backgroundView = NULL;
  }
}

void Play::init(Context &context)
{
  context.renderer->setBackgroundColor(0.0f, 0.0f, 0.0f);
  context.renderer->setSceneProjectionMode(GladeRenderer::ORTHO);
  //context.renderer->setDrawingOrderComparator(new Block.DrawingOrderComparator());
  
  levelScaleX = context.renderer->getViewportWidthCoords()  / 2;
  levelScaleY = context.renderer->getViewportHeightCoords() / 2;
  
  level = game_resource_manager->getLevel(levelInfo.path);

  std::shared_ptr<ShaderProgram> program =
      game_resource_manager->getShaderProgram("texcoord_frames.vertex.glsl", "textured.fragment.glsl");

  std::shared_ptr<Texture> backgroundTexture =
    game_resource_manager->getTexture(level->texturePackName, "background.png", "");
    
  if (backgroundTexture != nullptr) {
    backgroundView = new Drawable(Rectangle::INSTANCE, program);
    backgroundView->setTexture(backgroundTexture);
    background.addDrawable(backgroundView);
    backgroundView->getTransform()->setScale(levelScaleX, levelScaleY, 0);        
  }
  
  context.add(&background);
  
  blockWidth = blockHeight = min(
    context.renderer->getViewportWidthCoords()  / level->getWidthInBlocks(),
    context.renderer->getViewportHeightCoords() / level->getHeightInBlocks()
  );
  
  // initializing blocks and their game mechanics
  for (int blockX = 0; blockX < level->getWidthInBlocks(); ++blockX) {
    for (int blockY = 0; blockY < level->getHeightInBlocks(); ++blockY) {
      Level::Blocks *blocksInThisCell = level->getObjectsAt(blockX, blockY);
      Level::Blocks::iterator block;
      
      for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
        (*block)->initialize(level->texturePackName, blockWidth, blockHeight);
        applyStartingRulesForBlock(**block, blockX, blockY);
        //context.soundPlayer.hold(block->getSounds());
        context.add(*block);
      }
    }
  }
  
  // set actual speeds
  fallingSpeed    = BASE_FALLING_SPEED     * blockHeight;
  runningSpeed    = BASE_RUNNING_SPEED     * blockWidth;
  ladderUpSpeed   = BASE_LADDER_UP_SPEED   * blockHeight;
  ladderDownSpeed = BASE_LADDER_DOWN_SPEED * blockHeight;
  travelatorSpeed = BASE_TRAVELATOR_SPEED  * blockWidth;
}

void Play::applyStartingRulesForBlock(const Block &block, int block_x, int block_y)
{
  // TODO
}
