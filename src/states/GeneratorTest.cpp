#include <glade/Context.h>
#include <strug/BlockyArea.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/GeneratorTest.h>
#include <strug/controls/StrugController.h>

#define DEBUG_GENERATOR 1

extern Strug::ResourceManager *game_resource_manager;

const float GeneratorTest::BASE_RUNNING_SPEED = 0.5f;

class CharacterController: public StrugController
{
  private:
    GeneratorTest &playState;
    Context &context;
    
  public:
    CharacterController(Context &context_param, GeneratorTest &play_state):
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
          context.requestStateChange(std::unique_ptr<State>(new GeneratorTest()));
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

GeneratorTest::GeneratorTest():
  State(),
  screenScaleX(0),
  screenScaleY(0),
  controller(NULL),
  player(NULL),
  //generator(1438337493)
  generator()
{}

GeneratorTest::~GeneratorTest()
{
}

void GeneratorTest::init(Context &context)
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

  // Generate starting areas
  // addMoreAreas(context, playerAreaCoord, playerAreaCoord);
  
  
  addArea(context, 0, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  
  // Г-образные
   
/*  
  addArea(context, 0, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, 1, 1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context, 1, 0, MazeGenerator::PASSAGE_LEFT_TO_BOTTOM);

  addArea(context, 0, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, 1, -1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context, 1, 0, MazeGenerator::PASSAGE_LEFT_TO_TOP);
 
  addArea(context, 0, 0, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context, 1, 1, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, 0, 1, MazeGenerator::PASSAGE_TOP_TO_RIGHT);
 
  addArea(context, 1, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, 0, 1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context, 0, 0, MazeGenerator::PASSAGE_BOTTOM_TO_RIGHT);
*/

  // Ромб
/*
  addArea(context, 0, 0, MazeGenerator::PASSAGE_BOTTOM_TO_RIGHT);
  addArea(context, 1, 0, MazeGenerator::PASSAGE_LEFT_TO_BOTTOM);
  addArea(context, 1, 1, MazeGenerator::PASSAGE_LEFT_TO_TOP);
  addArea(context, 0, 1, MazeGenerator::PASSAGE_TOP_TO_RIGHT);
*/

  // Наклонная
/*
  addArea(context, 0, 0, MazeGenerator::PASSAGE_BOTTOM_TO_RIGHT);
  addArea(context, 1, 0, MazeGenerator::PASSAGE_LEFT_TO_TOP);
  addArea(context, 1, -1, MazeGenerator::PASSAGE_BOTTOM_TO_RIGHT);
  addArea(context, 2, -1, MazeGenerator::PASSAGE_LEFT_TO_TOP);
  addArea(context, 2, -2, MazeGenerator::PASSAGE_BOTTOM_TO_RIGHT);
*/ 

  // Т-образные
  /*
  addArea(context, 1, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, -1, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, 0, 1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context, 0, 0, MazeGenerator::PASSAGE_TCROSS_BLIND_TOP);

  addArea(context, 1, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, -1, 0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context, 0, -1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context, 0, 0, MazeGenerator::PASSAGE_TCROSS_BLIND_BOTTOM);

  addArea(context, -1,  0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context,  0,  1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context,  0, -1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context,  0,  0, MazeGenerator::PASSAGE_TCROSS_BLIND_RIGHT);
  
  addArea(context,  1,  0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context,  0,  1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context,  0, -1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context,  0,  0, MazeGenerator::PASSAGE_TCROSS_BLIND_LEFT);
*/

  // Крестообразный
  
/*  
  addArea(context,   1,  0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context,  -1,  0, MazeGenerator::PASSAGE_HORIZONTAL);
  addArea(context,   0,  1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context,   0, -1, MazeGenerator::PASSAGE_VERTICAL);
  addArea(context,   0,  0, MazeGenerator::PASSAGE_XCROSS);
*/  
  // Setup controls
  controller = new CharacterController(context, *this);
  context.setController(*controller);
}

void GeneratorTest::addArea(Context &context, int area_x, int area_y, MazeGenerator::CellType type)
{
  log("GENERATING AREA (%d, %d)", area_x, area_y);
  Area *area = new BlockyArea(Area::AREA_WIDTH_BLOCKS);
  generator.fillArea(area, area_x, area_y, type);
  
  // initializing blocks and their game mechanics
  for (int blockX = 0; blockX < area->getWidthInBlocks(); ++blockX) {
    for (int blockY = 0; blockY < area->getHeightInBlocks(); ++blockY) {
      Area::Blocks *blocksInThisCell = area->getObjectsAt(blockX, blockY);
      Area::Blocks::iterator block;
      
      for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
        (*block)->initialize(area->texturePackName, blockWidth, blockHeight);
        applyStartingRulesForBlock(**block, blockX + area_x * Area::AREA_WIDTH_BLOCKS, blockY + area_y * Area::AREA_WIDTH_BLOCKS);
        context.add(*block);
      }
    }
  }
  
  areaMap[std::pair<int,int>(area_x, area_y)] = area;
}

void GeneratorTest::applyStartingRulesForBlock(Block &block, int block_x, int block_y)
{
  block.getTransform()->setPosition(blockToWorldCoordX(block_x), blockToWorldCoordY(block_y), 0);
  
  if (block.getType() == Block::PLAYER) {
    player = (Player*) &block;
  }
}

void GeneratorTest::applyRules(Context &context)
{
  player->getTransform()->position->x += cameraMan.x * runningSpeed;
  player->getTransform()->position->y += cameraMan.y * runningSpeed;

  context.renderer->camera.position->x = player->getTransform()->position->x;
  context.renderer->camera.position->y = player->getTransform()->position->y;
  
  int playerBlockCoordX = getBlockCoordX(*player);
  int playerBlockCoordY = getBlockCoordY(*player);
  
  bool playerMovedToAnotherCell =
    playerBlockCoordX != prevPlayerBlockCoordX
    || playerBlockCoordY != prevPlayerBlockCoordY;
  
  if (playerMovedToAnotherCell) {
    int playerAreaCoordX  = areaCoordFromBlockCoord(playerBlockCoordX);
    int playerAreaCoordY  = areaCoordFromBlockCoord(playerBlockCoordY);
    
    bool playerMovedToAnotherArea =
      playerAreaCoordX != prevPlayerAreaCoordX
      || playerAreaCoordY != prevPlayerAreaCoordY;
    
    if (playerMovedToAnotherArea) {
      addMoreAreas(context, playerAreaCoordX, playerAreaCoordY);
      
      prevPlayerAreaCoordX = playerAreaCoordX;
      prevPlayerAreaCoordY = playerAreaCoordY;
    }
    
    prevPlayerBlockCoordX = playerBlockCoordX;
    prevPlayerBlockCoordY = playerBlockCoordY;
  }
}

void GeneratorTest::addMoreAreas(Context &context, int area_x, int area_y)
{
#if DEBUG_GENERATOR
  int areaXTo, areaYTo;
  int areaXFrom = areaXTo = area_x;
  int areaYFrom = areaYTo = area_y;
#else
  int areaXFrom = area_x - 1;
  int areaXTo   = area_x + 1;
  int areaYFrom = area_y - 1;
  int areaYTo   = area_y + 1;
#endif DEBUG_GENERATOR
  for (int i = areaXFrom; i <= areaXTo; ++i) {
    for (int j = areaYFrom; j <= areaYTo; ++j) {
      if (!areaMap.count(std::pair<int,int>(i, j))) {
        addArea(context, i, j, MazeGenerator::PASSAGE_HORIZONTAL);
      }
    }
  }
}

int GeneratorTest::getBlockCoordX(Block &object)
{
  return ::floor(((object.getTransform()->position->x + screenScaleX) / blockWidth));
}

int GeneratorTest::getBlockCoordY(Block &object)
{
  return ::floor(((object.getTransform()->position->y + screenScaleY) / blockHeight));
}

int GeneratorTest::areaCoordFromBlockCoord(int blockCoord)
{
  return blockCoord ? ::floor((float) blockCoord / Area::AREA_WIDTH_BLOCKS) : 0;
}

void GeneratorTest::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
