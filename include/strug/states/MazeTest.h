#pragma once

#include <map>
#include <vector>

#include <glade/GladeObject.h>
#include <glade/State.h>
#include <glade/math/Vector.h>
#include <glade/debug/log.h>
#include <strug/Level.h>
#include <strug/generator/WorldGenerator.h>

class Context;
class Block;
class StrugController;
class Player;

class MazeTest: public State
{
  friend class MazeController;
  
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
  
  private:
    static const float BASE_RUNNING_SPEED;
    static const int   MAZE_WIDTH, MAZE_HEIGHT;
    static const float BRAIDNESS;
    static const int   MAX_EXIT_CARVE_ATTEMPTS;
    
    float runningSpeed;
    float blockWidth, blockHeight;
    float screenScaleX, screenScaleY;
    Vector2i cameraMan;
    StrugController *controller;
    Player *player;

    bool
      leftExitCarved,
      rightExitCarved,
      topExitCarved,
      bottomExitCarved;
      
    typedef std::map< std::pair<int, int>, MazeCell > MazeMap;
    typedef std::vector< std::pair<int,int> > VectorOfIntPairs;
    
    MazeMap mazeMap;
    VectorOfIntPairs carvableCells;
    
    int
      prevPlayerBlockCoordX,
      prevPlayerBlockCoordY,
      prevPlayerAreaCoordX,
      prevPlayerAreaCoordY;

  public:
    MazeTest();
    ~MazeTest();
    
    virtual void init(Context &context);
    virtual void shutdown(Context &context);
    virtual void applyRules(Context &context);
    
    private:
      float blockToWorldCoordX(int blockX)
      {
        return blockX * blockWidth - screenScaleX + blockWidth / 2;
      }
      
      float blockToWorldCoordY(int blockY)
      {
        return blockY * blockHeight - screenScaleY + blockHeight / 2;
      }

      bool isEdgeCell(int cell_x, int cell_y)
      {
        return cell_x == 0 || cell_y == 0 || cell_x == MAZE_WIDTH - 1 || cell_y == MAZE_HEIGHT - 1;
      }
      
      void clearMaze()
      {
        mazeMap.clear();
        carvableCells.clear();
      }

      void dumpCarvableCells()
      {
        log("========== CARVABLE CELLS ==========:");
        
        for (VectorOfIntPairs::iterator i = carvableCells.begin(); i != carvableCells.end(); ++i) {
          log("(%d, %d)", i->first, i->second);
        }
        
        log("========== END CARVABLE CELLS ==========");
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
        
        return ((cell.passableNeighbors & MazeTest::TOP_NEIGHBOR) && ((cell.passableNeighbors & MazeTest::BOTTOM_NEIGHBOR) == 0))
          || ((cell.passableNeighbors & MazeTest::LEFT_NEIGHBOR) && ((cell.passableNeighbors & MazeTest::RIGHT_NEIGHBOR) == 0))
          || ((cell.passableNeighbors & MazeTest::BOTTOM_NEIGHBOR) && (cell.passableNeighbors & MazeTest::RIGHT_NEIGHBOR));
      }
      
      int getBlockCoordX(Block &object);
      int getBlockCoordY(Block &object);
      int areaCoordFromBlockCoord(int blockCoord);
      void applyStartingRulesForBlock(Block &block, int block_x, int block_y);
      void carvePassageAt(int area_x, int area_y);
      void createMaze();
      MazeCell updateNeighborCell(int carved_cell_x, int carved_cell_y, int neighbor_cell_x, int neighbor_cell_y);
      void addToCarvableList(int cell_x, int cell_y);
      void removeFromCarvableList(int cell_x, int cell_y);
      bool throwCoin(float probability = 0.5f); // move to separate module ("random")
      void carveExit(int &exit_x, int &exit_y, int adj_x_offset, int adj_y_offset, int &random_coord, int dim);
};
