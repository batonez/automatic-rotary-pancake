#include <algorithm>

#include <glade/Context.h>
#include <glade/render/Perception.h>
#include <strug/BlockyArea.h>
#include <strug/MonolithArea.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/Play.h>
#include <strug/controls/StrugController.h>

#define DEBUG_GENERATOR 0

extern Strug::ResourceManager *game_resource_manager;

const float Play::BASE_RUNNING_SPEED = 0.00001f;

class PlayController: public StrugController
{
  private:
    Play &playState;
    Context &context;
    
  public:
    PlayController(Context &context_param, Play &play_state):
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
  generator(),
  staticColShape(CollisionShape::STATIC),
  kinematicColShape(CollisionShape::KINEMATIC)
{}

Play::~Play()
{
}

void Play::init(Context &context)
{
  log("AREA WIDTH BLOCKS: %d", Area::AREA_WIDTH_BLOCKS);
  context.renderer->setBackgroundColor(0.0f, 1.0f, 0.0f);
  context.renderer->setSceneProjectionMode(Glade::Renderer::ORTHO);
  //context.renderer->setDrawingOrderComparator(new Block.DrawingOrderComparator());
  context.renderer->setPerception(new Perception());
  
  screenScaleX = context.renderer->getViewportWidthCoords()  / 2;
  screenScaleY = context.renderer->getViewportHeightCoords() / 2;
  
  blockWidth = blockHeight = 0.1f;
  log("BLOCK SIZE: %3.3f, %3.3f", blockWidth, blockHeight);
  
  // set actual speeds
  runningSpeed = BASE_RUNNING_SPEED * blockWidth;
  
  generator.createMazeCluster(Area::AREA_WIDTH_BLOCKS, Area::AREA_WIDTH_BLOCKS);
  
  std::pair<int,int> exitCoords =
    generator.mazeGenerator.getExit(::rand() % generator.mazeGenerator.getNumberOfExits());
  log("Spawn player at exit (area coords): %d, %d", exitCoords.first, exitCoords.second);
    
  // Create and initialize the player
  player = new Player();
  /*
  player->initialize("common", blockWidth, blockHeight);
  prevPlayerBlockCoordX = Area::AREA_WIDTH_BLOCKS * exitCoords.first  + Area::AREA_WIDTH_BLOCKS  / 2;
  prevPlayerBlockCoordY = Area::AREA_WIDTH_BLOCKS * exitCoords.second + Area::AREA_WIDTH_BLOCKS / 2;
  prevPlayerAreaCoordX  = areaCoordFromBlockCoord(prevPlayerBlockCoordX);
  prevPlayerAreaCoordY  = areaCoordFromBlockCoord(prevPlayerBlockCoordY);
  applyStartingRulesForBlock(*player, prevPlayerBlockCoordX, prevPlayerBlockCoordY);
  */
  context.add(player);
  /*
  addMoreAreas(context, prevPlayerAreaCoordX, prevPlayerAreaCoordY);
  
  context.getCollisionDetector()->addListener(&resolver);
  
  controller = new PlayController(context, *this);
  context.setController(*controller);
  */
  log("Play state init complete");
}

float Play::blockToWorldCoordXNotCentered(int blockX)
{
  return blockX * blockWidth - screenScaleX;
}

float Play::blockToWorldCoordYNotCentered(int blockY)
{
  return blockY * blockHeight - screenScaleY;
}

void Play::applyStartingRulesForMonolith(Block &block, int block_x, int block_y)
{
  block.getTransform()->setPosition(
    blockToWorldCoordXNotCentered(block_x),
    blockToWorldCoordYNotCentered(block_y),
    0
  );
  
  block.setCollisionShape(staticColShape);
}

void Play::applyStartingRulesForBlock(Block &block, int block_x, int block_y)
{
  block.getTransform()->setPosition(
    blockToWorldCoordX(block_x),
    blockToWorldCoordY(block_y),
    0
  );
  
  if (block.getType() == Block::PLAYER) {
    block.setCollisionShape(kinematicColShape);
  } else {
    block.setCollisionShape(staticColShape);
  }
}



void Play::applyRules(Context &context)
{
  // completely deleting areas that were previously archived into deletedAreaMap
  AreaMap::iterator i = deletedAreaMap.begin();
  while (i != deletedAreaMap.end()) {
    delete i->second;
    i = deletedAreaMap.erase(i);
  }
  
  // Moving player character
  player->getTransform()->position->x += cameraMan.x * runningSpeed;
  player->getTransform()->position->y += cameraMan.y * runningSpeed;

  if (player->getPhysicBody()) {
    player->getPhysicBody()->acceleration.x = cameraMan.x * runningSpeed;
    player->getPhysicBody()->acceleration.y = cameraMan.y * runningSpeed;
  }
  
  // camera at the player
  if (context.renderer->getCamera() != nullptr) {
    context.renderer->getCamera()->position->x = player->getTransform()->position->x;
    context.renderer->getCamera()->position->y = player->getTransform()->position->y;
  }
  
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
      //addMoreAreas(context, playerAreaCoordX, playerAreaCoordY);
      
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
#endif // DEBUG_GENERATOR
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
  log("=== player is at (%d, %d)", player_area_x, player_area_y);
  log("=== total areas %d", areaMap.size());
  log("====== CURRENT AREAS ====");
  AreaMap::iterator i = areaMap.begin();
  
  while (i != areaMap.end()) {
    std::pair<int,int> areaCoords = i->first;
    log("(%d, %d)", areaCoords.first, areaCoords.second);

#if DEBUG_GENERATOR
    if (areaCoords.first != player_area_x || areaCoords.second != player_area_y) {
#else
    if (abs(areaCoords.first - player_area_x) > 1 || abs(areaCoords.second - player_area_y) > 1) {
#endif
      Area *area = i->second;
      
      if (area->getType() == Area::AREA_BLOCKY) {
        log("REMOVING BLOCKY AREA AT: %d, %d", areaCoords.first, areaCoords.second);
        for (int x = 0; x < area->getWidthInBlocks(); ++x) {
          for (int y = 0; y < area->getHeightInBlocks(); ++y) {
            Area::Blocks *blocksInThisCell = area->getObjectsAt(x, y);
            Area::Blocks::iterator block;
            
            for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
              context.remove(*block);
            }
          }
        }
      } else {
        Area::Blocks *blocksInThisCell = area->getObjectsAt(0, 0);
        Area::Blocks::iterator block;
        
        log("REMOVING MONOLITH AREA AT: %d, %d", areaCoords.first, areaCoords.second);
        for (block = blocksInThisCell->begin(); block != blocksInThisCell->end(); ++block) {
          log("1 BLOCK");
          context.remove(*block);
          break;
        }
      }
      
      i = areaMap.erase(i);
      deletedAreaMap[areaCoords] = area;
    } else {
      ++i;
    }
  }
  
  log("====== END CURRENT AREAS ======");
}

void Play::addArea(Context &context, int area_x, int area_y, MazeGenerator::CellType type)
{
  Area *area = NULL;
  
  if (type == MazeGenerator::PASSAGE_NO) {
    log("ADDIN MONOLITH AREA AT %d, %d", area_x, area_y);
    area = new MonolithArea(Area::AREA_WIDTH_BLOCKS);
    Area::Blocks *blocksInThisCell = area->getObjectsAt(0, 0);
    Block *block = blocksInThisCell->at(0);
    block->initialize(area->texturePackName, Area::AREA_WIDTH_BLOCKS * blockWidth, Area::AREA_WIDTH_BLOCKS * blockHeight);
    applyStartingRulesForMonolith(
      *block, 
      (Area::AREA_WIDTH_BLOCKS / 2) + area_x * Area::AREA_WIDTH_BLOCKS,
      (Area::AREA_WIDTH_BLOCKS / 2) + area_y * Area::AREA_WIDTH_BLOCKS
    );
    
    context.add(block);
  } else {
    area = new BlockyArea(Area::AREA_WIDTH_BLOCKS);
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
  return blockCoord ? ::floor((float) blockCoord / Area::AREA_WIDTH_BLOCKS) : 0;
}

void Play::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
