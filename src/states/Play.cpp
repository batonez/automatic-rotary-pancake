#include <algorithm>

#include <glade/Context.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/Play.h>
#include <strug/controls/StrugController.h>

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
  player(NULL)
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
  
  generator.mazeGenerator.createMaze();

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
          addArea(context, i, j, WorldGenerator::AREA_FULL);
          continue;
        }
        
        assert(cell.passableStraightNeighborsNumber > 0);
        
        if (cell.passableStraightNeighborsNumber == 4) {
          addArea(context, i, j, WorldGenerator::PASSAGE_XCROSS);
        } else if (cell.passableStraightNeighborsNumber == 3) {
          if (!(cell.passableNeighbors & MazeGenerator::TOP_NEIGHBOR)) {
            addArea(context, i, j, WorldGenerator::PASSAGE_TCROSS_BLIND_TOP);
          } else if (!(cell.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR)) {
            addArea(context, i, j, WorldGenerator::PASSAGE_TCROSS_BLIND_BOTTOM);
          } else if (!(cell.passableNeighbors & MazeGenerator::LEFT_NEIGHBOR)) {
            addArea(context, i, j, WorldGenerator::PASSAGE_TCROSS_BLIND_LEFT);
          } else if (!(cell.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR)) {
            addArea(context, i, j, WorldGenerator::PASSAGE_TCROSS_BLIND_RIGHT);
          }
        } else if (cell.passableStraightNeighborsNumber <= 2) {
          if (cell.passableNeighbors & MazeGenerator::LEFT_NEIGHBOR) {
            if (cell.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) {
              addArea(context, i, j, WorldGenerator::PASSAGE_HORIZONTAL);
            } else if (cell.passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
              addArea(context, i, j, WorldGenerator::PASSAGE_LEFT_TO_TOP);
            } else if (cell.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) {
              addArea(context, i, j, WorldGenerator::PASSAGE_LEFT_TO_BOTTOM);
            } else {
              addArea(context, i, j, WorldGenerator::PASSAGE_HORIZONTAL); // тупик
            }
          } else if (cell.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) {
            if (cell.passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
              addArea(context, i, j, WorldGenerator::PASSAGE_TOP_TO_RIGHT);
            } else if (cell.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) {
              addArea(context, i, j, WorldGenerator::PASSAGE_BOTTOM_TO_RIGHT);
            } else {
              addArea(context, i, j, WorldGenerator::PASSAGE_HORIZONTAL); // тупик
            }
          } else if (cell.passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
            addArea(context, i, j, WorldGenerator::PASSAGE_VERTICAL);  // тупик или просто вертикальный проход
          } else if (cell.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) {
            addArea(context, i, j, WorldGenerator::PASSAGE_VERTICAL);  // тупик
          }
        }
      }
    }
  }
}

void Play::addArea(Context &context, int area_x, int area_y, WorldGenerator::AreaType type)
{
  Area *area = new Area(Level::AREA_WIDTH_BLOCKS);
  generator.fillArea(area, areaMap, area_x, area_y, type);
  
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
