#include <glade/Context.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/Play.h>
#include <strug/controls/StrugController.h>

#define DEBUG_GENERATOR 0

extern Strug::ResourceManager *game_resource_manager;

const float Play::BASE_RUNNING_SPEED = 0.2f;
const int   Play::AREA_WIDTH_BLOCKS = 10;

class CharacterController: public StrugController
{
  private:
    Play &playState;
    
  public:
    CharacterController(Play &play_state):
      StrugController(),
      playState(play_state)
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

Play::Play():
  State(),
  screenScaleX(0),
  screenScaleY(0),
  controller(NULL),
  player(NULL)
{}

Play::~Play()
{
}

void Play::init(Context &context)
{
  context.renderer->setBackgroundColor(0.0f, 0.0f, 0.0f);
  context.renderer->setSceneProjectionMode(GladeRenderer::ORTHO);
  //context.renderer->setDrawingOrderComparator(new Block.DrawingOrderComparator());
  
  screenScaleX = context.renderer->getViewportWidthCoords()  / 2;
  screenScaleY = context.renderer->getViewportHeightCoords() / 2;
  
  blockWidth = blockHeight = min(
    context.renderer->getViewportWidthCoords()  / AREA_WIDTH_BLOCKS,
    context.renderer->getViewportHeightCoords() / AREA_WIDTH_BLOCKS
  );
  
  // set actual speeds
  runningSpeed = BASE_RUNNING_SPEED * blockWidth;
  
  Player *playerCharacter = new Player();
  playerCharacter->initialize("common", blockWidth, blockHeight);
  applyStartingRulesForBlock(*playerCharacter, AREA_WIDTH_BLOCKS / 2, AREA_WIDTH_BLOCKS / 2);
  context.add(playerCharacter);

 controller = new CharacterController(*this);
  context.setController(*controller);
}

void Play::addArea(Context &context, int area_x, int area_y)
{
  log("GENERATING AREA (%d, %d)", area_x, area_y);
  Area *area = new Area(AREA_WIDTH_BLOCKS);
  generator.fillArea(area);
  
  // initializing blocks and their game mechanics
  for (int blockX = 0; blockX < area->getWidthInBlocks(); ++blockX) {
    for (int blockY = 0; blockY < area->getHeightInBlocks(); ++blockY) {
      Level::Blocks *blocksInThisCell = area->getObjectsAt(blockX, blockY);
      Level::Blocks::iterator block;
      
      for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
        (*block)->initialize(area->texturePackName, blockWidth, blockHeight);
        applyStartingRulesForBlock(**block, blockX + area_x * AREA_WIDTH_BLOCKS, blockY + area_y * AREA_WIDTH_BLOCKS);
        context.add(*block);
      }
    }
  }
  
  areaMap[std::pair<int,int>(area_x, area_y)] = area;
}

void Play::applyStartingRulesForBlock(Block &block, int block_x, int block_y)
{
  block.getTransform()->setPosition(blockToWorldCoordX(block_x), blockToWorldCoordY(block_y), 0);
  
  if (block.getType() == Block::PLAYER) {
    player = (Player*) &block;
  }
}

void Play::applyRules(Context &context)
{
  player->getTransform()->position->x += cameraMan.x * runningSpeed;
  player->getTransform()->position->y += cameraMan.y * runningSpeed;

  context.renderer->camera.position->x = player->getTransform()->position->x;
  context.renderer->camera.position->y = player->getTransform()->position->y;
  
  int playerBlockCoordX = getBlockCoordX(*player);
  int playerBlockCoordY = getBlockCoordY(*player);
  int playerAreaCoordX  = areaCoordFromBlockCoord(playerBlockCoordX);
  int playerAreaCoordY  = areaCoordFromBlockCoord(playerBlockCoordY);
  
#if DEBUG_GENERATOR
  int areaXTo, areaYTo;
  int areaXFrom = areaXTo = playerAreaCoordX;
  int areaYFrom = areaYTo = playerAreaCoordY;
#else
  int areaXFrom = playerAreaCoordX - 1;
  int areaXTo = playerAreaCoordX + 1;
  int areaYFrom = playerAreaCoordY - 1;
  int areaYTo = playerAreaCoordY + 1;
#endif DEBUG_GENERATOR
 
  for (int i = areaXFrom; i <= areaXTo; ++i) {
    for (int j = areaYFrom; j <= areaYTo; ++j) {
      if (!areaMap.count(std::pair<int,int>(i, j))) {
        addArea(context, i, j);
      }
    }
  }
}

int Play::getBlockCoordX(Block &object)
{
  return ::floor(((object.getTransform()->position->x + screenScaleX) / blockWidth));
}

int Play::getBlockCoordY(Block &object)
{
  return ::floor(((object.getTransform()->position->y + screenScaleY) / blockHeight));
}

int Play::areaCoordFromBlockCoord(int blockCoord)
{
  return blockCoord ? ::floor((float) blockCoord / AREA_WIDTH_BLOCKS) : 0;
}

void Play::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
