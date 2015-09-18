#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include <limits.h>

#include <sha1.h>

#include <glade/debug/log.h>
#include <glade/math/vector.h>
#include <strug/Area.h>
#include <strug/blocks/Terrain.h>
#include <strug/generator/WorldGenerator.h>
#include <strug/exception/StrugException.h>
#include <strug/generator/SimpleGenerator.h>
#include <strug/generator/StraightPassage.h>
#include <strug/generator/PassageTurn.h>
#include <strug/generator/TCross.h>
#include <strug/generator/XCross.h>

#define MIN_PASSAGE_HEIGHT 4
#define MAX_PASSAGE_HEIGHT 14

struct AreaStitching
{
  int leftExitTopTerrainHeight;
  int leftExitBottomTerrainHeight;
  
  int rightExitTopTerrainHeight;
  int rightExitBottomTerrainHeight;
  
  int topExitLeftTerrainWidth;
  int topExitRightTerrainWidth;
  
  int bottomExitLeftTerrainWidth;
  int bottomExitRightTerrainWidth;
};

static std::map<std::pair<int, int>, AreaStitching> stitchingMap;

static unsigned int getAreaSeed(long seed_param, int x_param, int y_param)
{
  struct CombinedData
  {
     long seed;
     int  x;
     int  y;
  } data = {seed_param, x_param, y_param };
  
  union Sha1Digest
  {
    unsigned char chars[20];
    unsigned int combinedSeed;
  } result;
  
  sha1::calc(&data, sizeof(data), result.chars);

  return result.combinedSeed;
}

WorldGenerator::WorldGenerator(unsigned int seed_param)
{
  setSeed(seed_param);
}

void WorldGenerator::setSeed(unsigned int seed_param)
{
  seed = seed_param ? seed_param : (unsigned int) ::time(NULL);
  log("World generator seed is %u", seed);
}

static std::pair<int,int> generatePassageThickness(int area_size)
{
  int firstTerrainThickness = ::rand() % (area_size - MAX_PASSAGE_HEIGHT - 1) + 1;
  int spaceLeft = area_size - firstTerrainThickness;
  int secondTerrainThickness    = ::rand() % (MAX_PASSAGE_HEIGHT - MIN_PASSAGE_HEIGHT) + spaceLeft - MAX_PASSAGE_HEIGHT;
  
  return std::pair<int,int>(firstTerrainThickness, secondTerrainThickness);
}

void WorldGenerator::createMazeCluster(int area_width, int area_height)
{
  ::srand(seed);
  mazeGenerator.createMaze();
  
  for (int i = 0; i < MazeGenerator::MAZE_WIDTH; ++i) {
    for (int j = 0; j < MazeGenerator::MAZE_HEIGHT; ++j) {
      MazeGenerator::MazeCell cell = mazeGenerator.getCellAt(i, j);
      
      if (!cell.passable) {
        continue;
      }
      
      AreaStitching neighborStitching = {0};
      AreaStitching currentCellStitching = {0};
      
      if (cell.passableNeighbors & MazeGenerator::LEFT_NEIGHBOR) {
        try {
          neighborStitching = stitchingMap.at(std::pair<int,int>(i - 1, j));
        } catch (std::out_of_range &e) {}
        
        if (neighborStitching.rightExitTopTerrainHeight) {
          currentCellStitching.leftExitTopTerrainHeight = neighborStitching.rightExitTopTerrainHeight;
          currentCellStitching.leftExitBottomTerrainHeight = neighborStitching.rightExitBottomTerrainHeight;
        } else {
          std::pair<int,int> passage = generatePassageThickness(area_height);
          currentCellStitching.leftExitBottomTerrainHeight = passage.first;
          currentCellStitching.leftExitTopTerrainHeight = passage.second;
        }
      }
      
      if (cell.passableNeighbors & MazeGenerator::RIGHT_NEIGHBOR) {
        try {
          neighborStitching = stitchingMap.at(std::pair<int,int>(i + 1, j));
        } catch (std::out_of_range &e) {}
        
        if (neighborStitching.leftExitTopTerrainHeight) {
          currentCellStitching.rightExitTopTerrainHeight = neighborStitching.leftExitTopTerrainHeight;
          currentCellStitching.rightExitBottomTerrainHeight = neighborStitching.leftExitBottomTerrainHeight;
        } else {
          std::pair<int,int> passage = generatePassageThickness(area_height);
          currentCellStitching.rightExitBottomTerrainHeight = passage.first;
          currentCellStitching.rightExitTopTerrainHeight = passage.second;
        }
      }
      
      if (cell.passableNeighbors & MazeGenerator::TOP_NEIGHBOR) {
        try {
          neighborStitching = stitchingMap.at(std::pair<int,int>(i, j - 1));
        } catch (std::out_of_range &e) {}
        
        if (neighborStitching.bottomExitLeftTerrainWidth) {
          currentCellStitching.topExitLeftTerrainWidth = neighborStitching.bottomExitLeftTerrainWidth;
          currentCellStitching.topExitRightTerrainWidth = neighborStitching.bottomExitRightTerrainWidth;
        } else {
          std::pair<int,int> passage = generatePassageThickness(area_width);
          currentCellStitching.topExitLeftTerrainWidth = passage.first;
          currentCellStitching.topExitRightTerrainWidth = passage.second;
        }
      }
      
      if (cell.passableNeighbors & MazeGenerator::BOTTOM_NEIGHBOR) {
        try {
          neighborStitching = stitchingMap.at(std::pair<int,int>(i, j + 1));
        } catch (std::out_of_range &e) {}
        
        if (neighborStitching.topExitLeftTerrainWidth) {
          currentCellStitching.bottomExitLeftTerrainWidth = neighborStitching.topExitLeftTerrainWidth;
          currentCellStitching.bottomExitRightTerrainWidth = neighborStitching.topExitRightTerrainWidth;
        } else {
          std::pair<int,int> passage = generatePassageThickness(area_width);
          currentCellStitching.bottomExitLeftTerrainWidth = passage.first;
          currentCellStitching.bottomExitRightTerrainWidth = passage.second;
        }
      }
      
      stitchingMap[std::pair<int,int>(i, j)] = currentCellStitching;
    }
  }
}

void WorldGenerator::fillArea(Area *area, int area_x, int area_y, MazeGenerator::CellType type)
{
  unsigned int combinedSeed = getAreaSeed(seed, area_x, area_y);
  log("Combined seed for this area is (%u + %d + %d) = %u", seed, area_x, area_y, combinedSeed);
  ::srand(combinedSeed);
  
  area->texturePackName = "cave";

  AreaStitching stitching = {0};
  
  try {
    stitching = stitchingMap.at(std::pair<int,int>(area_x, area_y));
  } catch (std::out_of_range &e) {}

  switch (type)
  {
    case MazeGenerator::PASSAGE_HORIZONTAL:
    case MazeGenerator::PASSAGE_HORIZONTAL_BLIND_LEFT:
    case MazeGenerator::PASSAGE_HORIZONTAL_BLIND_RIGHT:
      StraightPassage::createStraightPassage( 
        area,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.leftExitTopTerrainHeight,
        stitching.leftExitBottomTerrainHeight,
        stitching.rightExitTopTerrainHeight,
        stitching.rightExitBottomTerrainHeight
      );
      break;
    case MazeGenerator::PASSAGE_VERTICAL:
    case MazeGenerator::PASSAGE_VERTICAL_BLIND_TOP:
    case MazeGenerator::PASSAGE_VERTICAL_BLIND_BOTTOM:
      StraightPassage::createStraightPassage( 
        area,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.bottomExitLeftTerrainWidth,
        stitching.bottomExitRightTerrainWidth,
        stitching.topExitLeftTerrainWidth,
        stitching.topExitRightTerrainWidth
      );
      break;
    case MazeGenerator::PASSAGE_LEFT_TO_BOTTOM:
    case MazeGenerator::PASSAGE_BOTTOM_TO_LEFT:
      PassageTurn::createPassageTurn(
        area,
        false,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.leftExitTopTerrainHeight,
        stitching.leftExitBottomTerrainHeight,
        stitching.bottomExitLeftTerrainWidth,
        stitching.bottomExitRightTerrainWidth
      );
      break;
    case MazeGenerator::PASSAGE_LEFT_TO_TOP:
    case MazeGenerator::PASSAGE_TOP_TO_LEFT:
      PassageTurn::createPassageTurn(
        area,
        false,
        false,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.leftExitTopTerrainHeight,
        stitching.leftExitBottomTerrainHeight,
        stitching.topExitLeftTerrainWidth,
        stitching.topExitRightTerrainWidth
      );
      break;
    case MazeGenerator::PASSAGE_BOTTOM_TO_RIGHT:
    case MazeGenerator::PASSAGE_RIGHT_TO_BOTTOM:
      PassageTurn::createPassageTurn(
        area,
        false,
        true,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.rightExitTopTerrainHeight,
        stitching.rightExitBottomTerrainHeight,
        stitching.bottomExitRightTerrainWidth,
        stitching.bottomExitLeftTerrainWidth
      );
      break;
    case MazeGenerator::PASSAGE_TOP_TO_RIGHT:
    case MazeGenerator::PASSAGE_RIGHT_TO_TOP:
      PassageTurn::createPassageTurn(
        area,
        true,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.topExitLeftTerrainWidth,
        stitching.topExitRightTerrainWidth,
        stitching.rightExitTopTerrainHeight,
        stitching.rightExitBottomTerrainHeight
      );
      break;
    case MazeGenerator::PASSAGE_TCROSS_BLIND_TOP:
      TCross::createPassageTurn(
        area,
        false,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.leftExitTopTerrainHeight,
        stitching.leftExitBottomTerrainHeight,
        stitching.rightExitTopTerrainHeight,
        stitching.rightExitBottomTerrainHeight,
        stitching.bottomExitLeftTerrainWidth,
        stitching.bottomExitRightTerrainWidth
      );
      break;
    case MazeGenerator::PASSAGE_TCROSS_BLIND_BOTTOM:
      TCross::createPassageTurn(
        area,
        false,
        false,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.leftExitTopTerrainHeight,
        stitching.leftExitBottomTerrainHeight,
        stitching.rightExitBottomTerrainHeight,
        stitching.rightExitTopTerrainHeight,
        stitching.topExitLeftTerrainWidth,
        stitching.topExitRightTerrainWidth
      );
      break;
    case MazeGenerator::PASSAGE_TCROSS_BLIND_RIGHT:
      TCross::createPassageTurn(
        area,
        true,
        false,
        true,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.topExitLeftTerrainWidth,
        stitching.topExitRightTerrainWidth,
        stitching.bottomExitRightTerrainWidth,
        stitching.bottomExitLeftTerrainWidth,
        stitching.leftExitTopTerrainHeight,
        stitching.leftExitBottomTerrainHeight
      );
      break;
    case MazeGenerator::PASSAGE_TCROSS_BLIND_LEFT:
      TCross::createPassageTurn(
        area,
        true,
        false,
        false,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.topExitLeftTerrainWidth,
        stitching.topExitRightTerrainWidth,
        stitching.bottomExitLeftTerrainWidth,
        stitching.bottomExitRightTerrainWidth,
        stitching.rightExitTopTerrainHeight,
        stitching.rightExitBottomTerrainHeight
      );
      break;
    case MazeGenerator::PASSAGE_XCROSS:
      XCross::createPassageTurn(
        area,
        MAX_PASSAGE_HEIGHT,
        MIN_PASSAGE_HEIGHT,
        stitching.leftExitTopTerrainHeight,
        stitching.leftExitBottomTerrainHeight,
        stitching.rightExitTopTerrainHeight,
        stitching.rightExitBottomTerrainHeight,
        stitching.bottomExitLeftTerrainWidth,
        stitching.bottomExitRightTerrainWidth,
        stitching.topExitLeftTerrainWidth,
        stitching.topExitRightTerrainWidth
      );
      break;
    case MazeGenerator::PASSAGE_NO:
      SimpleGenerator::fillAll(area);
      break;
    default:
      throw StrugException("Unknown passage type");
  }
}
