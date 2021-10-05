#include <algorithm>

#include <glade/Context.h>
#include <strug/Area.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/controls/StrugController.h>
#include <strug/states/CollisionTest.h>

#define DEBUG_GENERATOR 0

extern Strug::ResourceManager *game_resource_manager;

const float CollisionTest::BASE_RUNNING_SPEED = 0.00001f;

class MazeController: public StrugController
{
  private:
    Context &context;
    CollisionTest &playState;
    
  public:
    MazeController(Context &context_param, CollisionTest &play_state):
      StrugController(),
      playState(play_state),
      context(context_param)
    {}
    
    bool buttonPress(int id, int terminalId)
    {
      bool handled = false;
      
      switch (id) {
        case StrugController::BUTTON_LEFT:
          playState.cameraMan.x = -1;
          handled = true;
          break;
        case StrugController::BUTTON_RIGHT:
          playState.cameraMan.x =  1;
          handled = true;
          break;
        case StrugController::BUTTON_UP:
          playState.cameraMan.y = -1;
          handled = true;
          break;
        case StrugController::BUTTON_DOWN:
          playState.cameraMan.y =  1;
          handled = true;
          break;
        case StrugController::BUTTON_FIRE:
          handled = true;
          break;
      }
      
      return handled;
    }
  
    bool buttonRelease(int id, int terminalId)
    {
      bool handled = false;
      
      switch (id) {
        case StrugController::BUTTON_LEFT:
          playState.cameraMan.x = 0;
          handled = true;
          break;
        case StrugController::BUTTON_RIGHT:
          playState.cameraMan.x = 0;
          handled = true;
          break;
        case StrugController::BUTTON_UP:
          playState.cameraMan.y = 0;
          handled = true;
          break;
        case StrugController::BUTTON_DOWN:
          playState.cameraMan.y = 0;
          handled = true;
          break;
      }
      
      return handled;
    }
    
    void init()
    {  
    }
};

CollisionTest::CollisionTest():
  State(),
  screenScaleX(0),
  screenScaleY(0),
  controller(NULL),
  player(NULL),
  terrain(NULL),
  staticColShape(CollisionShape::STATIC),
  kinematicColShape(CollisionShape::KINEMATIC)
{}

CollisionTest::~CollisionTest()
{
}

void CollisionTest::init(Context &context)
{
  log("AREA WIDTH BLOCKS: %d", Area::AREA_WIDTH_BLOCKS);
  context.renderer->setBackgroundColor(0.0f, 0.0f, 0.0f);
  context.renderer->setSceneProjectionMode(Glade::Renderer::ORTHO);
  //context.renderer->setDrawingOrderComparator(new Block.DrawingOrderComparator());
  
  screenScaleX = context.renderer->getViewportWidthCoords()  / 2;
  screenScaleY = context.renderer->getViewportHeightCoords() / 2;
  
  //blockWidth = blockHeight = min(
  //  context.renderer->getViewportWidthCoords()  / Area::AREA_WIDTH_BLOCKS,
  //  context.renderer->getViewportHeightCoords() / Area::AREA_WIDTH_BLOCKS
  //);
  
  blockWidth = blockHeight = 0.1f;
  
  log("BLOCK SIZE: %3.3f, %3.3f", blockWidth, blockHeight);
  
  // set actual speeds
  runningSpeed = BASE_RUNNING_SPEED * blockWidth;
    
  // Create and initialize the player
  player = new Player();
  player->initialize("common", blockWidth, blockHeight);
  player->setName("Player");
  applyStartingRulesForBlock(*player, 1, 1);
  context.add(player);
  
  terrain = new Terrain();
  terrain->setName("Terrain");
  terrain->initialize("cave", blockWidth, blockHeight);
  applyStartingRulesForBlock(*terrain, 4, 4);
  context.add(terrain);
  
  //context.getCollisionDetector()->addListener(&resolver);
  
  controller = new MazeController(context, *this);
  context.setController(*controller);
}

void CollisionTest::applyStartingRulesForBlock(Block &block, int block_x, int block_y)
{
  block.getTransform()->setPosition(blockToWorldCoordX(block_x), blockToWorldCoordY(block_y), 0);
  
  if (block.getType() == Block::PLAYER) {
    block.setCollisionShape(kinematicColShape);
  } else {
    block.setCollisionShape(staticColShape);
  }
}

void CollisionTest::applyRules(Context &context)
{
  // Moving player character
  //player->getTransform()->position->x += cameraMan.x * runningSpeed * context.timer.getDeltaTime();
  //player->getTransform()->position->y += cameraMan.y * runningSpeed * context.timer.getDeltaTime();

//  player->getPhysicBody()->velocity.x = cameraMan.x * runningSpeed;
//  player->getPhysicBody()->velocity.y = cameraMan.y * runningSpeed;
  
  player->getPhysicBody()->acceleration.x = cameraMan.x * runningSpeed;
  player->getPhysicBody()->acceleration.y = cameraMan.y * runningSpeed;
  
  context.renderer->getCamera()->position->x = player->getTransform()->position->x;
  context.renderer->getCamera()->position->y = player->getTransform()->position->y;
}

int CollisionTest::getBlockCoordX(Block &object)
{
  return ::floor(((object.getTransform()->position->x + screenScaleX) / blockWidth));
}

int CollisionTest::getBlockCoordY(Block &object)
{
  return ::floor(((object.getTransform()->position->y + screenScaleY) / blockHeight));
}

int CollisionTest::areaCoordFromBlockCoord(int blockCoord)
{
  return blockCoord ? ::floor((float) blockCoord / Area::AREA_WIDTH_BLOCKS) : 0;
}

void CollisionTest::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
