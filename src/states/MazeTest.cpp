#include <algorithm>

#include <glade/Context.h>
#include <strug/Area.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/MazeTest.h>
#include <strug/generator/MazeGenerator.h>
#include <strug/controls/StrugController.h>

extern Strug::ResourceManager *game_resource_manager;

const float MazeTest::BASE_RUNNING_SPEED = 0.5f;

class MazeController: public StrugController
{
  private:
    MazeTest &playState;
    Context &context;
    
  public:
    MazeController(Context &context_param, MazeTest &play_state):
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
          log("FIRE!");
          context.requestStateChange(std::unique_ptr<State>(new MazeTest()));
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

MazeTest::MazeTest():
  State(),
  screenScaleX(0),
  screenScaleY(0),
  controller(NULL),
  player(NULL)
{}

MazeTest::~MazeTest()
{
}

void MazeTest::init(Context &context)
{
  log("AREA WIDTH BLOCKS: %d", Area::AREA_WIDTH_BLOCKS);
  context.renderer->setBackgroundColor(0.0f, 0.0f, 0.0f);
  context.renderer->setSceneProjectionMode(GladeRenderer::ORTHO);
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
  Player *playerCharacter = new Player();
  playerCharacter->initialize("common", blockWidth, blockHeight);
  int playerBlockCoord = Area::AREA_WIDTH_BLOCKS / 2;
  int playerAreaCoord  = areaCoordFromBlockCoord(playerBlockCoord);
  applyStartingRulesForBlock(*playerCharacter, playerBlockCoord, playerBlockCoord);
  prevPlayerBlockCoordX = prevPlayerBlockCoordY = playerBlockCoord;
  prevPlayerAreaCoordX = prevPlayerAreaCoordY = playerAreaCoord;
  context.add(playerCharacter);

  mazeGenerator.createMaze();
  
  for (int i = 0; i < MazeGenerator::MAZE_WIDTH; ++i) {
    for (int j = 0; j < MazeGenerator::MAZE_HEIGHT; ++j) {
      if (!mazeGenerator.isCellPassable(i, j)) {
        Block *block = new Terrain();
        block->initialize("cave", blockWidth, blockHeight);
        applyStartingRulesForBlock(*block, i, j);
        context.add(block);
      }
    }
  }
  
  controller = new MazeController(context, *this);
  context.setController(*controller);
}

void MazeTest::applyStartingRulesForBlock(Block &block, int block_x, int block_y)
{
  block.getTransform()->setPosition(blockToWorldCoordX(block_x), blockToWorldCoordY(block_y), 0);
  
  if (block.getType() == Block::PLAYER) {
    player = (Player*) &block;
  }
}

void MazeTest::applyRules(Context &context)
{
  player->getTransform()->position->x += cameraMan.x * runningSpeed;
  player->getTransform()->position->y += cameraMan.y * runningSpeed;

  context.renderer->camera.position->x = player->getTransform()->position->x;
  context.renderer->camera.position->y = player->getTransform()->position->y;
}

int MazeTest::getBlockCoordX(Block &object)
{
  return ::floor(((object.getTransform()->position->x + screenScaleX) / blockWidth));
}

int MazeTest::getBlockCoordY(Block &object)
{
  return ::floor(((object.getTransform()->position->y + screenScaleY) / blockHeight));
}

int MazeTest::areaCoordFromBlockCoord(int blockCoord)
{
  return blockCoord ? ::floor((float) blockCoord / Area::AREA_WIDTH_BLOCKS) : 0;
}

void MazeTest::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
