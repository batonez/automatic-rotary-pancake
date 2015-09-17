#include <glade/Context.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/Play.h>
#include <strug/controls/StrugController.h>

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
  Area *area = new Area(AREA_WIDTH_BLOCKS);
  generator.fillArea(area);
  
  // initializing blocks and their game mechanics
  for (int i = 0; i < area->getWidthInBlocks(); ++i) {
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {
      Level::Blocks *blocksInThisCell = area->getObjectsAt(i, j);
      Level::Blocks::iterator block;
      
      for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
        (*block)->initialize(area->texturePackName, blockWidth, blockHeight);
        applyStartingRulesForBlock(**block, (i+1) * area_x, (j+1) * area_y);
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
  
  log("%d, %d", areaCoordFromBlockCoord(playerBlockCoordX), areaCoordFromBlockCoord(playerBlockCoordY));
  //log("%d, %d", playerBlockCoordX, playerBlockCoordY);
  
  if (!areaMap.count(std::pair<int,int>(1, 1))) {
    addArea(context, 1, 1);
  }
}

int Play::getBlockCoordX(Block &object)
{
  float rawBlockCoord = (object.getTransform()->position->x + screenScaleX) / blockWidth;
  return rawBlockCoord >= 0 ? ::ceil(rawBlockCoord) : ::floor(rawBlockCoord);
}

int Play::getBlockCoordY(Block &object)
{
  float rawBlockCoord = (object.getTransform()->position->y + screenScaleY) / blockHeight;
  return rawBlockCoord >= 0 ? ::ceil(rawBlockCoord) : ::floor(rawBlockCoord);
}

int Play::areaCoordFromBlockCoord(int blockCoord)
{
  float rawAreaCoord = (float) blockCoord / AREA_WIDTH_BLOCKS;
  return rawAreaCoord >= 0 ? ::ceil(rawAreaCoord) : ::floor(rawAreaCoord);
}

void Play::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
    