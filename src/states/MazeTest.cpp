#include <algorithm>

#include <glade/Context.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/states/MazeTest.h>
#include <strug/controls/StrugController.h>

extern Strug::ResourceManager *game_resource_manager;

const float MazeTest::BASE_RUNNING_SPEED = 0.5f;
const int   MazeTest::MAZE_WIDTH  = 16;
const int   MazeTest::MAZE_HEIGHT = 16;

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
  
  // Create and initialize the player
  Player *playerCharacter = new Player();
  playerCharacter->initialize("common", blockWidth, blockHeight);
  int playerBlockCoord = Level::AREA_WIDTH_BLOCKS / 2;
  int playerAreaCoord  = areaCoordFromBlockCoord(playerBlockCoord);
  applyStartingRulesForBlock(*playerCharacter, playerBlockCoord, playerBlockCoord);
  prevPlayerBlockCoordX = prevPlayerBlockCoordY = playerBlockCoord;
  prevPlayerAreaCoordX = prevPlayerAreaCoordY = playerAreaCoord;
  context.add(playerCharacter);

  createMaze();
  
  for (int i = 0; i < MAZE_WIDTH; ++i) {
    for (int j = 0; j < MAZE_HEIGHT; ++j) {
      bool passable = false;
      
      try {
        MazeCell cell = mazeMap.at(std::pair<int,int>(i, j));
        passable = cell.passable;
      } catch (std::out_of_range &e) {}
      
      if (!passable) {
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

void MazeTest::createMaze()
{
  clearMaze();
  
  // Generating entrance
  int entranceX, entranceY;
  
  if (rand() % 2) {
    entranceX = rand() % 2 ? 0 : MAZE_WIDTH - 1;
    entranceY = rand() % (MAZE_HEIGHT - 2) + 1;
  } else {
    entranceX = rand() % (MAZE_WIDTH - 2) + 1;
    entranceY = rand() % 2 ? 0 : MAZE_HEIGHT - 1;
  }
  
  carvePassageAt(entranceX, entranceY);
  log("Entrance: (%d, %d)", entranceX, entranceY);
  // End Generating entrance
  
  // Carving the shit out of this maze
  while (!carvableCells.empty()) {
    dumpCarvableCells();
    int whereToCarve = rand() % carvableCells.size();
    log("Rolled carve at vector index: %d", whereToCarve);
    
    carvePassageAt(
      carvableCells[whereToCarve].first,
      carvableCells[whereToCarve].second
    );
  }
}

void MazeTest::carvePassageAt(int cell_x, int cell_y)
{
  assert(cell_x < MAZE_WIDTH);
  assert(cell_y < MAZE_HEIGHT);
  
  log("Carving at (%d, %d)", cell_x, cell_y);
  
  MazeCell carvedCell = {0};
  
  try {
    carvedCell = mazeMap.at(std::pair<int,int>(cell_x, cell_y));
  } catch (std::out_of_range &e) {}
  
  carvedCell.passable = true;
  removeFromCarvableList(cell_x, cell_y);
  
  for (int i = cell_x - 1; i <= cell_x + 1; ++i) {
    if (i < 0) {
      continue;
    }
    
    if (i >= MAZE_WIDTH) {
      break;
    }
    
    for (int j = cell_y - 1; j <= cell_y + 1; ++j) {
      if (j < 0) {
        continue;
      }
      
      if (j >= MAZE_WIDTH) {
        break;
      }
      
      if (i == cell_x && j == cell_y) {
        continue;
      }
      
      // Update all cells that are adjancent to the freshly carved one
      MazeCell neighbor = updateNeighborCell(cell_x, cell_y, i, j);
      
      // Cells that have more than one straight neighbors are not carvable
      if (neighbor.passableStraightNeighborsNumber == 2) {
        removeFromCarvableList(i, j);
        continue;
      }
      
      log("Should we remove neighbor (%d, %d) from carvable list? His passable neighbors is %d, and straight neighbor num is %d",
        i, j, neighbor.passableNeighbors, neighbor.passableStraightNeighborsNumber);
      log("Mask test (top, left, bottom, right): %d, %d, %d, %d", neighbor.passableNeighbors & MazeTest::TOP_NEIGHBOR, neighbor.passableNeighbors & MazeTest::LEFT_NEIGHBOR, neighbor.passableNeighbors & MazeTest::BOTTOM_NEIGHBOR, neighbor.passableNeighbors & MazeTest::RIGHT_NEIGHBOR);
      
      // Cells that have standalone corner neighbors are not carvable
      if (neighbor.passableNeighbors & MazeTest::TOPLEFT_NEIGHBOR) {
        log("Found a topleft carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeTest::TOP_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeTest::LEFT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }
      
      if (neighbor.passableNeighbors & MazeTest::TOPRIGHT_NEIGHBOR) {
        log("Found a topright carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeTest::TOP_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeTest::RIGHT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }

      if (neighbor.passableNeighbors & MazeTest::BOTTOMRIGHT_NEIGHBOR) {
        log("Found a bottomright carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeTest::BOTTOM_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeTest::RIGHT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }
      
      if (neighbor.passableNeighbors & MazeTest::BOTTOMLEFT_NEIGHBOR) {
        log("Found a bottomleft carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeTest::BOTTOM_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeTest::LEFT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }
      
      // Cells that have exactly one straight neighbor and are not on the edge of the maze are carvable
      if (neighbor.passableStraightNeighborsNumber == 1 && !isEdgeCell(i, j) && !neighbor.passable) {
        addToCarvableList(i, j);
      }
    }
  }
  
  mazeMap[std::pair<int,int>(cell_x, cell_y)] = carvedCell;
}

void MazeTest::addToCarvableList(int cell_x, int cell_y)
{
  VectorOfIntPairs::iterator cellCoords =
    std::find(carvableCells.begin(), carvableCells.end(), std::pair<int,int>(cell_x, cell_y));

  if (cellCoords == carvableCells.end()) {
    carvableCells.push_back(std::pair<int,int>(cell_x, cell_y));
  }
}

void MazeTest::removeFromCarvableList(int cell_x, int cell_y)
{
  log("Removing (%d, %d) from carvable list", cell_x, cell_y);
  
  VectorOfIntPairs::iterator cellCoords =
    std::find(carvableCells.begin(), carvableCells.end(), std::pair<int,int>(cell_x, cell_y));

  if (cellCoords != carvableCells.end()) {
    carvableCells.erase(cellCoords);
  }
}

MazeTest::MazeCell MazeTest::updateNeighborCell(int carved_cell_x, int carved_cell_y, int neighbor_cell_x, int neighbor_cell_y)
{
  assert(neighbor_cell_x >= 0 && neighbor_cell_x < MAZE_WIDTH);
  assert(neighbor_cell_y >= 0 && neighbor_cell_y < MAZE_HEIGHT);
  
  // fetch the cell from the maze map or create if not exists
  MazeCell neighborCell = {0};
  
  try {
    neighborCell = mazeMap.at(std::pair<int,int>(neighbor_cell_x, neighbor_cell_y));
  } catch (std::out_of_range &e) {}
  
  // update cell struct members
  int xDiff =  carved_cell_x - neighbor_cell_x;
  int yDiff =  carved_cell_y - neighbor_cell_y;
  
  log("Updating neighbors... carved cell (%d, %d), neighbor cell (%d, %d), xDiff = %d, yDiff = %d",
    carved_cell_x, carved_cell_y, neighbor_cell_x, neighbor_cell_y, xDiff, yDiff);
  
  assert(xDiff != 0 || yDiff != 0);
  
  if (xDiff > 0 && yDiff > 0) {
    neighborCell.passableNeighbors |= MazeTest::BOTTOMRIGHT_NEIGHBOR;
  } else if (xDiff > 0 && yDiff == 0) {
    neighborCell.passableNeighbors |= MazeTest::RIGHT_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff > 0 && yDiff < 0) {
    neighborCell.passableNeighbors |= MazeTest::TOPRIGHT_NEIGHBOR;
  } else if (xDiff == 0 && yDiff > 0) {
    neighborCell.passableNeighbors |= MazeTest::BOTTOM_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff == 0 && yDiff < 0) {
    neighborCell.passableNeighbors |= MazeTest::TOP_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff < 0 && yDiff > 0) {
    neighborCell.passableNeighbors |= MazeTest::BOTTOMLEFT_NEIGHBOR;
  } else if (xDiff < 0 && yDiff == 0) {
    neighborCell.passableNeighbors |= MazeTest::LEFT_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff < 0 && yDiff < 0) {
    neighborCell.passableNeighbors |= MazeTest::TOPLEFT_NEIGHBOR;
  }
  
  // save the cell into the maze map
  mazeMap[std::pair<int,int>(neighbor_cell_x, neighbor_cell_y)] = neighborCell;
  
  // return a copy of the cell
  return neighborCell;
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
  return blockCoord ? ::floor((float) blockCoord / Level::AREA_WIDTH_BLOCKS) : 0;
}

void MazeTest::shutdown(Context &context)
{
  // TODO free level and other memory
  
  //context.setController(NULL);
  
  if (controller) {
    delete controller;
  }
}
