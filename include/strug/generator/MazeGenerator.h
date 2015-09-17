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
    
    enum CellType
    {
      // unpassable cell
      PASSAGE_NO,
      // surrounded by unpassable cells
      PASSAGE_ISOLATED_ROOM,
      // straight
      PASSAGE_HORIZONTAL,
      PASSAGE_HORIZONTAL_BLIND_LEFT,
      PASSAGE_HORIZONTAL_BLIND_RIGHT,
      PASSAGE_VERTICAL,
      PASSAGE_VERTICAL_BLIND_TOP,
      PASSAGE_VERTICAL_BLIND_BOTTOM,
      // Turns
      PASSAGE_LEFT_TO_BOTTOM, PASSAGE_BOTTOM_TO_LEFT,
      PASSAGE_LEFT_TO_TOP, PASSAGE_TOP_TO_LEFT,
      PASSAGE_BOTTOM_TO_RIGHT, PASSAGE_RIGHT_TO_BOTTOM,
      PASSAGE_TOP_TO_RIGHT, PASSAGE_RIGHT_TO_TOP,
      // Intersections
      PASSAGE_TCROSS_BLIND_TOP,
      PASSAGE_TCROSS_BLIND_BOTTOM,
      PASSAGE_TCROSS_BLIND_LEFT,
      PASSAGE_TCROSS_BLIND_RIGHT,
      PASSAGE_XCROSS,
      // Default
      PASSAGE_UNKNOWN,
    };
    
    class MazeCell
    {
      public:
        bool passable;
        // bitmask of NEIGHBOR_CELLS
        unsigned char passableNeighbors;
        unsigned char passableStraightNeighborsNumber;
        
      private:
        CellType type;
      
      public:
        MazeCell():
          passable(false),
          passableNeighbors(0),
          passableStraightNeighborsNumber(0),
          type(MazeGenerator::PASSAGE_UNKNOWN)
        {}
        
        CellType getType()
        {
          if (type != MazeGenerator::PASSAGE_UNKNOWN) {
            return type;
          }
          
          if (!passable) {
            type = MazeGenerator::PASSAGE_NO;
          } else if (passableStraightNeighborsNumber == 4) {
            type = MazeGenerator::PASSAGE_XCROSS;
          } else if (passableStraightNeighborsNumber == 3) {
            if (!(passableNeighbors & MazeGenerator::TOP_NEIGHBOR)) {
              type = MazeGenerator::PASSAGE_TCROSS_BLIND_TOP;
            } else if (!(passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR)) {
              type = MazeGenerator::PASSAGE_TCROSS_BLIND_BOTTOM;
            } else if (!(passableNeighbors & MazeGenerator::LEFT_NEIGHBOR)) {
              type = MazeGenerator::PASSAGE_TCROSS_BLIND_LEFT;
            } else if (!(passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR)) {
              type = MazeGenerator::PASSAGE_TCROSS_BLIND_RIGHT;
            }
          } else if (passableStraightNeighborsNumber == 2) {
            if (passableNeighbors & MazeGenerator::LEFT_NEIGHBOR) {
              if (passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) {
                type = MazeGenerator::PASSAGE_HORIZONTAL;
              } else if (passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
                type = MazeGenerator::PASSAGE_LEFT_TO_TOP;
              } else if (passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) {
                type = MazeGenerator::PASSAGE_LEFT_TO_BOTTOM;
              } else {
                
              }
            } else if (passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) {
              if (passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
                type = MazeGenerator::PASSAGE_TOP_TO_RIGHT;
              } else if (passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) {
                type = MazeGenerator::PASSAGE_BOTTOM_TO_RIGHT;
              }
            } else if (passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
              type = MazeGenerator::PASSAGE_VERTICAL;
            }
          } else if (passableStraightNeighborsNumber == 1) {
            if (passableNeighbors & MazeGenerator::LEFT_NEIGHBOR) {
              type = MazeGenerator::PASSAGE_HORIZONTAL_BLIND_RIGHT;
            } else if (passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) {
              type = MazeGenerator::PASSAGE_HORIZONTAL_BLIND_LEFT;
            } else if (passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
              type = MazeGenerator::PASSAGE_VERTICAL_BLIND_BOTTOM;
            } else if (passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) {
              type = MazeGenerator::PASSAGE_VERTICAL_BLIND_TOP;
            }
          } else if (passableStraightNeighborsNumber == 0) {
            type = MazeGenerator::PASSAGE_ISOLATED_ROOM;
          }
          
          return type;
        }
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
