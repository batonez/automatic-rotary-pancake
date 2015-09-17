#include <algorithm>
#include <assert.h>

#include <glade/math/util.h>
#include <strug/Level.h>
#include <strug/ResourceManager.h>
#include <strug/blocks/Terrain.h>
#include <strug/blocks/Player.h>
#include <strug/generator/MazeGenerator.h>
#include <strug/controls/StrugController.h>

extern Strug::ResourceManager *game_resource_manager;

const int   MazeGenerator::MAZE_WIDTH  = 16;
const int   MazeGenerator::MAZE_HEIGHT = 16;
const float MazeGenerator::BRAIDNESS   = 0.5f;
const int   MazeGenerator::MAX_EXIT_CARVE_ATTEMPTS = 40;

MazeGenerator::MazeGenerator():
  topExitCarved(false),
  bottomExitCarved(false),
  leftExitCarved(false),
  rightExitCarved(false)
{}

MazeGenerator::~MazeGenerator()
{
}

void MazeGenerator::createMaze()
{
  clearMaze();
  
  // Generating entrance
  int entranceX, entranceY;
  
  if (rand() % 2) {
    entranceX = rand() % 2 ? 0 : MAZE_WIDTH - 1;
    entranceY = rand() % (MAZE_HEIGHT - 2) + 1;
    
    rightExitCarved = (bool) entranceX;
    leftExitCarved  = !rightExitCarved;
    
  } else {
    entranceX = rand() % (MAZE_WIDTH - 2) + 1;
    entranceY = rand() % 2 ? 0 : MAZE_HEIGHT - 1;
    
    bottomExitCarved = (bool) entranceY;
    topExitCarved    = !bottomExitCarved;
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

  // Carving exits
    
  int exitX, exitY;
  int adjXOffset, adjYOffset;
  int dim;
  
  if (!leftExitCarved) {
    exitX = 0;
    adjXOffset = 1;
    adjYOffset = 0;
    int &randomCoord = exitY;  
    dim = MAZE_HEIGHT;
    carveExit(exitX, exitY, adjXOffset, adjYOffset, randomCoord, dim);
  }
    
  if (!rightExitCarved) {
    exitX = MAZE_WIDTH - 1;
    adjXOffset = -1;
    adjYOffset = 0;
    int &randomCoord = exitY;  
    dim = MAZE_HEIGHT;
    carveExit(exitX, exitY, adjXOffset, adjYOffset, randomCoord, dim);
  }
    
  if (!topExitCarved) {
    exitY = 0;
    adjXOffset = 0;
    adjYOffset = 1;
    int &randomCoord = exitX;  
    dim = MAZE_WIDTH;
    carveExit(exitX, exitY, adjXOffset, adjYOffset, randomCoord, dim);
  }
    
  if (!bottomExitCarved) {
    exitY = MAZE_HEIGHT - 1;
    adjXOffset = 0;
    adjYOffset = -1;
    int &randomCoord = exitX;  
    dim = MAZE_WIDTH;
    carveExit(exitX, exitY, adjXOffset, adjYOffset, randomCoord, dim);
  }
}

void MazeGenerator::carveExit(int &exit_x, int &exit_y, int adj_x_offset, int adj_y_offset, int &random_coord, int dim)
{
  for (int i = 0; i < MAX_EXIT_CARVE_ATTEMPTS; ++i) {
    random_coord = ::rand() % (dim - 1) + 1;
    
    MazeCell cell = {0};
    
    try {
      cell = mazeMap.at(std::pair<int,int>(exit_x + adj_x_offset, exit_y + adj_y_offset));
    } catch (std::out_of_range &e) {}
 
    if (cell.passable) {
      carvePassageAt(exit_x, exit_y);
      break;
    }
  }
}

void MazeGenerator::carvePassageAt(int cell_x, int cell_y)
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
      if (cellHasCheckerCarvedNeighbors(neighbor) || (neighbor.passableStraightNeighborsNumber == 2 && !::throw_coin(BRAIDNESS))) {
        removeFromCarvableList(i, j);
        continue;
      }
      
      log("Should we remove neighbor (%d, %d) from carvable list? His passable neighbors is %d, and straight neighbor num is %d",
        i, j, neighbor.passableNeighbors, neighbor.passableStraightNeighborsNumber);
      log("Mask test (top, left, bottom, right): %d, %d, %d, %d", neighbor.passableNeighbors & MazeGenerator::TOP_NEIGHBOR, neighbor.passableNeighbors & MazeGenerator::LEFT_NEIGHBOR, neighbor.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR, neighbor.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR);
      
      // Cells that have standalone corner neighbors are not carvable
      if (neighbor.passableNeighbors & MazeGenerator::TOPLEFT_NEIGHBOR) {
        log("Found a topleft carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeGenerator::TOP_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeGenerator::LEFT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }
      
      if (neighbor.passableNeighbors & MazeGenerator::TOPRIGHT_NEIGHBOR) {
        log("Found a topright carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeGenerator::TOP_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }

      if (neighbor.passableNeighbors & MazeGenerator::BOTTOMRIGHT_NEIGHBOR) {
        log("Found a bottomright carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }
      
      if (neighbor.passableNeighbors & MazeGenerator::BOTTOMLEFT_NEIGHBOR) {
        log("Found a bottomleft carved neighbor for (%d, %d)", i, j);
        if ((neighbor.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) == 0
          && (neighbor.passableNeighbors & MazeGenerator::LEFT_NEIGHBOR) == 0) {
            removeFromCarvableList(i, j);
            continue;
        }
      }
      
      // Cells that have exactly one straight neighbor and are not on the edge of the maze are carvable
      if (neighbor.passableStraightNeighborsNumber <= 2 && !isEdgeCell(i, j) && !neighbor.passable) {
        addToCarvableList(i, j);
      }
    }
  }
  
  mazeMap[std::pair<int,int>(cell_x, cell_y)] = carvedCell;
}

void MazeGenerator::addToCarvableList(int cell_x, int cell_y)
{
  VectorOfIntPairs::iterator cellCoords =
    std::find(carvableCells.begin(), carvableCells.end(), std::pair<int,int>(cell_x, cell_y));

  if (cellCoords == carvableCells.end()) {
    carvableCells.push_back(std::pair<int,int>(cell_x, cell_y));
  }
}

void MazeGenerator::removeFromCarvableList(int cell_x, int cell_y)
{
  log("Removing (%d, %d) from carvable list", cell_x, cell_y);
  
  VectorOfIntPairs::iterator cellCoords =
    std::find(carvableCells.begin(), carvableCells.end(), std::pair<int,int>(cell_x, cell_y));

  if (cellCoords != carvableCells.end()) {
    carvableCells.erase(cellCoords);
  }
}

MazeGenerator::MazeCell MazeGenerator::updateNeighborCell(int carved_cell_x, int carved_cell_y, int neighbor_cell_x, int neighbor_cell_y)
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
    neighborCell.passableNeighbors |= MazeGenerator::BOTTOMRIGHT_NEIGHBOR;
  } else if (xDiff > 0 && yDiff == 0) {
    neighborCell.passableNeighbors |= MazeGenerator::RIGHT_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff > 0 && yDiff < 0) {
    neighborCell.passableNeighbors |= MazeGenerator::TOPRIGHT_NEIGHBOR;
  } else if (xDiff == 0 && yDiff > 0) {
    neighborCell.passableNeighbors |= MazeGenerator::BOTTOM_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff == 0 && yDiff < 0) {
    neighborCell.passableNeighbors |= MazeGenerator::TOP_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff < 0 && yDiff > 0) {
    neighborCell.passableNeighbors |= MazeGenerator::BOTTOMLEFT_NEIGHBOR;
  } else if (xDiff < 0 && yDiff == 0) {
    neighborCell.passableNeighbors |= MazeGenerator::LEFT_NEIGHBOR;
    ++neighborCell.passableStraightNeighborsNumber;
  } else if (xDiff < 0 && yDiff < 0) {
    neighborCell.passableNeighbors |= MazeGenerator::TOPLEFT_NEIGHBOR;
  }
  
  // save the cell into the maze map
  mazeMap[std::pair<int,int>(neighbor_cell_x, neighbor_cell_y)] = neighborCell;
  
  // return a copy of the cell
  return neighborCell;
}

bool MazeGenerator::isCellPassable(int cell_x, int cell_y)
{
  bool result = false;
  
  try {
    MazeCell cell = mazeMap.at(std::pair<int,int>(cell_x, cell_y));
    result = cell.passable;
  } catch (std::out_of_range &e) {}
  
  return result;
}
