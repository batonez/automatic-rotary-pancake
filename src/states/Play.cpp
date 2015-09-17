#include <algorithm>

#include <glade/Context.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/Play.h>
#include <strug/controls/StrugController.h>

#define DEBUG_GENERATOR 1

extern Strug::ResourceManager *game_resource_manager;

const float Play::BASE_RUNNING_SPEED = 0.5f;

class MazeController: public StrugController
{
  private:
    Play &playState;
    Context &context;
    
  public:
    MazeController(Context &context_param, Play &play_state):
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
          context.requestStateChange(std::unique_ptr<State>(new Play()));
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
  player(NULL),
  generator(31337)
{}

Play::~Play()
{
}

void Play::init(Context &context)
{
  log("AREA WIDTH BLOCKS: %d", Level::AREA_WIDTH_BLOCKS);
  context.renderer->setBackgroundColor(0.0f, 0.0f, 0.0f);
  context.renderer->setSceneProjectionMode(GladeRenderer::ORTHO);
  //context.renderer->setDrawingOrderComparator(new Block.DrawingOrderComparator());
  
  screenScaleX = context.renderer->getViewportWidthCoords()  / 2;
  screenScaleY = context.renderer->getViewportHeightCoords() / 2;
  
  //blockWidth = blockHeight = min(
  //  context.renderer->getViewportWidthCoords()  / Level::AREA_WIDTH_BLOCKS,
  //  context.renderer->getViewportHeightCoords() / Level::AREA_WIDTH_BLOCKS
  //);
  
  blockWidth = blockHeight = 0.1f;
  
  log("BLOCK SIZE: %3.3f, %3.3f", blockWidth, blockHeight);
  
  // set actual speeds
  runningSpeed = BASE_RUNNING_SPEED * blockWidth;
  
  generator.createMazeCluster(Level::AREA_WIDTH_BLOCKS, Level::AREA_WIDTH_BLOCKS);
  
  std::pair<int,int> exitCoords =
    generator.mazeGenerator.getExit(::rand() % generator.mazeGenerator.getNumberOfExits());
  log("Spawn player at exit (area coords): %d, %d", exitCoords.first, exitCoords.second);
    
  // Create and initialize the player
  Player *playerCharacter = new Player();
  playerCharacter->initialize("common", blockWidth, blockHeight);
  prevPlayerBlockCoordX = Level::AREA_WIDTH_BLOCKS * exitCoords.first  + Level::AREA_WIDTH_BLOCKS  / 2;
  prevPlayerBlockCoordY = Level::AREA_WIDTH_BLOCKS * exitCoords.second + Level::AREA_WIDTH_BLOCKS / 2;
  prevPlayerAreaCoordX  = areaCoordFromBlockCoord(prevPlayerBlockCoordX);
  prevPlayerAreaCoordY  = areaCoordFromBlockCoord(prevPlayerBlockCoordY);
  applyStartingRulesForBlock(*playerCharacter, prevPlayerBlockCoordX, prevPlayerBlockCoordY);
  context.add(playerCharacter);
  
  addMoreAreas(context, prevPlayerAreaCoordX, prevPlayerAreaCoordY);
  
  controller = new MazeController(context, *this);
  context.setController(*controller);
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
      removeFarAreas(context, playerAreaCoordX, playerAreaCoordY);
      addMoreAreas(context, playerAreaCoordX, playerAreaCoordY);
      
      prevPlayerAreaCoordX = playerAreaCoordX;
      prevPlayerAreaCoordY = playerAreaCoordY;
    }
    
    prevPlayerBlockCoordX = playerBlockCoordX;
    prevPlayerBlockCoordY = playerBlockCoordY;
  }
}

void Play::addMoreAreas(Context &context, int area_x, int area_y)
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
        MazeGenerator::MazeCell cell = generator.mazeGenerator.getCellAt(i, j);
        
        if (!cell.passable) {
          addArea(context, i, j, MazeGenerator::PASSAGE_NO);
          continue;
        }
        
        MazeGenerator::CellType areaType = cell.getType();
        assert(areaType != MazeGenerator::PASSAGE_ISOLATED_ROOM);
        addArea(context, i, j, areaType);
      }
    }
  }
}

void Play::removeFarAreas(Context &context, int player_area_x, int player_area_y)
{
  AreaMap::iterator i;
  log("=== player is at (%d, %d)", player_area_x, player_area_y);
  log("=== total areas %d", areaMap.size());
  log("====== CURRENT AREAS ====");
  
  for (i = areaMap.begin(); i != areaMap.end(); ++i) {
    std::pair<int,int> areaCoords = i->first;
    log("(%d, %d)", areaCoords.first, areaCoords.second);
    
    if (areaCoords.first != player_area_x || areaCoords.second != player_area_y) {
      Area *area = i->second;
      
      for (int x = 0; x < area->getWidthInBlocks(); ++x) {
        for (int y = 0; y < area->getHeightInBlocks(); ++y) {
          Level::Blocks *blocksInThisCell = area->getObjectsAt(x, y);
          Level::Blocks::iterator block;
          
          for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
            context.remove(*block);
          }
        }
      }
     
      areaMap.erase(i);
      delete area;
    }
  }
  
  log("====== END CURRENT AREAS ======");
}

void Play::addArea(Context &context, int area_x, int area_y, MazeGenerator::CellType type)
{
  Area *area = new Area(Level::AREA_WIDTH_BLOCKS);
  generator.fillArea(area, area_x, area_y, type);
  
  // initializing blocks and their game mechanics
  for (int blockX = 0; blockX < area->getWidthInBlocks(); ++blockX) {
    for (int blockY = 0; blockY < area->getHeightInBlocks(); ++blockY) {
      Level::Blocks *blocksInThisCell = area->getObjectsAt(blockX, blockY);
      Level::Blocks::iterator block;
      
      for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
        (*block)->initialize(area->texturePackName, blockWidth, blockHeight);
        applyStartingRulesForBlock(**block, blockX + area_x * Level::AREA_WIDTH_BLOCKS, blockY + area_y * Level::AREA_WIDTH_BLOCKS);
        context.add(*block);
      }
    }
  }
  
  areaMap[std::pair<int,int>(area_x, area_y)] = area;
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
  return blockCoord ? ::floor((float) blockCoord / Level::AREA_WIDTH_BLOCKS) : 0;
}

void Play::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
