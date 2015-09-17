//#include <glade/physics/CollisionEventListener.h>
//#include <glade/math/Graph.h>
#include <glade/Context.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>

#include <strug/states/Play.h>

extern Strug::ResourceManager *game_resource_manager;

const float Play::BASE_RUNNING_SPEED = 0.004f;

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
/*
  std::shared_ptr<Texture> backgroundTexture =
    game_resource_manager->getTexture(level->texturePackName, "background.png", "");
    
  if (backgroundTexture != nullptr) {
    backgroundView = new Drawable(Rectangle::INSTANCE, program);
    backgroundView->setTexture(backgroundTexture);
    background.addDrawable(backgroundView);
    backgroundView->getTransform()->setScale(levelScaleX, levelScaleY, 0);        
  }
  
  context.add(&background);
  */
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
  runningSpeed = BASE_RUNNING_SPEED * blockWidth;
}

void Play::applyStartingRulesForBlock(const Block &block, int block_x, int block_y)
{
  // TODO
}
