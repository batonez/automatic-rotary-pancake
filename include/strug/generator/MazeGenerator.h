#pragma once

#include <map>
#include <vector>

#include <glade/math/Vector.h>
#include <glade/debug/log.h>
#include <strug/exception/StrugException.h>

class MazeGenerator
{
  public:
    enum NEIGHBOR_CELLS
    {
      LEFT_NEIGHBOR        = 1,
      RIGHT_NEIGHBOR       = 2,
      TOP_NEIGHBOR         = 4,
      BOTTOM_NEIGHBOR      = 8,
      TOPLEFT_NEIGHBOR     = 16,
      TOPRIGHT_NEIGHBOR    = 32,
      BOTTOMLEFT_NEIGHBOR  = 64,
      BOTTOMRIGHT_NEIGHBOR = 128,
    };
    
    struct MazeCell
    {
      bool passable;
      // bitmask of NEIGHBOR_CELLS
      unsigned char passableNeighbors;
      unsigned char passableStraightNeighborsNumber;
    };

    static const int   MAZE_WIDTH, MAZE_HEIGHT;
    
  private:
    static const float BRAIDNESS;
    static const int   MAX_EXIT_CARVE_ATTEMPTS;

    bool
      leftExitCarved,
      rightExitCarved,
      topExitCarved,
      bottomExitCarved;

    typedef std::map< std::pair<int, int>, MazeCell > MazeMap;
    typedef std::vector< std::pair<int,int> > VectorOfIntPairs;

    MazeMap mazeMap;
    VectorOfIntPairs carvableCells;
    VectorOfIntPairs exits;

  public:
    MazeGenerator();
    ~MazeGenerator();
    
    void createMaze();
    bool isCellPassable(int cell_x, int cell_y);
    MazeCell getCellAt(int cell_x, int cell_y);
    
    int getNumberOfExits()
    {
      return exits.size();
    }
    
    std::pair<int,int> getExit(int index)
    {
      try {
        return exits.at(index);
      } catch (std::out_of_range &e) {
        throw StrugException("Exit index is out of range");
      }
    }

  private:
    bool isEdgeCell(int cell_x, int cell_y)
    {
      return cell_x == 0 || cell_y == 0 || cell_x == MAZE_WIDTH - 1 || cell_y == MAZE_HEIGHT - 1;
    }
    
    void clearMaze()
    {
      mazeMap.clear();
      carvableCells.clear();
      exits.clear();
    }
    
    // True example (X - this cell, O - carved cell, * - uncarved cell):
    // ***
    // OX*
    // *O*
    bool cellHasCheckerCarvedNeighbors(MazeCell &cell)
    {
      if (cell.passableStraightNeighborsNumber <= 1) {
        return false;
      }

      if (cell.passableStraightNeighborsNumber > 2) {
        return true;
      }
      
      return ((cell.passableNeighbors & MazeGenerator::TOP_NEIGHBOR) && ((cell.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) == 0))
        || ((cell.passableNeighbors & MazeGenerator::LEFT_NEIGHBOR) && ((cell.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) == 0))
        || ((cell.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) && (cell.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR));
    }
    
    void carvePassageAt(int area_x, int area_y);
    MazeCell updateNeighborCell(int carved_cell_x, int carved_cell_y, int neighbor_cell_x, int neighbor_cell_y);
    void addToCarvableList(int cell_x, int cell_y);
    void removeFromCarvableList(int cell_x, int cell_y);
    void carveExit(int &exit_x, int &exit_y, int adj_x_offset, int adj_y_offset, int &random_coord, int dim);
    void dumpCarvableCells();
};
