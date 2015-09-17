#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#include <glade/debug/log.h>
#include <glade/math/vector.h>
#include <strug/generator/PassageTurn.h>
#include <strug/blocks/Terrain.h>
#include <strug/exception/StrugException.h>

int notNegative(int val)
{
  return val < 0 ? 0 : val;
}

Vector2i PassageTurn::getConstrainedSteepness(
  int area_width,
  int min_height_this_col,
  int max_height_this_col,
  int from_height,
  int to_height,
  int current_stripe_number,
  int max_steepness)
{
  Vector2i result(max_steepness, max_steepness);
  log("S %d %d", result.x, result.y);
  
  // Cannot let the passage be taller than max passage height  
  result.x = std::min<int>(notNegative(from_height - min_height_this_col), result.x);  
  // Exclude corner blocking
  result.y = std::min<int>(notNegative(max_height_this_col - from_height), result.y);
  
  log("S %d %d", result.x, result.y);
  
  if (!from_height || !to_height) {
    return result;
  }
  
  int colsLeft = area_width - current_stripe_number;
  int maxFinalHeight = from_height + colsLeft * max_steepness;
  int minFinalHeight = from_height - colsLeft * max_steepness;
  int maxRequiredFinalHeight = to_height;
  int minRequiredFinalHeight = to_height;
  
  log("S maxFinalHeight = %d = %d + %d * %d", maxFinalHeight, from_height, colsLeft, max_steepness);
  log("S minFinalHeight = %d = %d - %d * %d", minFinalHeight, from_height, colsLeft, max_steepness);
  log("S maxRequiredFinalHeight = %d = %d + %d", maxRequiredFinalHeight, to_height, max_steepness);
  log("S minRequiredFinalHeight = %d = %d - %d", minRequiredFinalHeight, to_height, max_steepness);
  
  if (maxFinalHeight <=  maxRequiredFinalHeight) {
    log("S Forcing steepness up");
    result.x = -result.y;
  } else if (minFinalHeight >= minRequiredFinalHeight) {
    log("S Forcing steepness down");
    result.y = -result.x;
  }
  
  log("S %d %d", result.x, result.y);
  
  log("S %d %d", result.x, result.y);
  
  return result;
}

void PassageTurn::calculateStraightPassageStripe(
  /*in-out*/ int &bottom_terrain_height,
  /*in-out*/ int &top_terrain_height,
  int to_left_width,
  int to_right_width,
  int area_width,
  int area_height,
  int current_stripe_number,
  int max_passage_thickness,
  int min_passage_thickness,
  int max_top_steepness,
  int max_bottom_steepness)
{
  assert(bottom_terrain_height >= 0);
  assert(top_terrain_height >= 0);

  log("Old terrain bottom: %d, top: %d", bottom_terrain_height, top_terrain_height);
  
  int oldBottomHeight = bottom_terrain_height;
  int oldTopHeight    = top_terrain_height;
  
  if (current_stripe_number >= to_left_width) {
    bottom_terrain_height = 0;
    log("Bottom terrain height is set to zero (after turn)");
  } else {    
    // Min and max passage height
    int maxHeightThisCol = area_height - oldTopHeight - min_passage_thickness;
    int minHeightThisCol = 1;
    
    log("Bottom terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
    
    // generating bottom terrain
    log("Bottom steepness:");
    Vector2i bottomSteepness = getConstrainedSteepness(to_left_width, minHeightThisCol, maxHeightThisCol, oldBottomHeight, 1, current_stripe_number, max_bottom_steepness);

    bottom_terrain_height = 
      ::rand() % (bottomSteepness.x + bottomSteepness.y + 1) + oldBottomHeight - bottomSteepness.x;
  }
  
  int spaceLeft = area_height - bottom_terrain_height;
  
  log("Generateed bottom terrain: %d, Space left: %d", bottom_terrain_height, spaceLeft);
  
  if (current_stripe_number >= area_width - to_right_width) {
    top_terrain_height = area_height;
    log("Top terrain height is set to max (after turn)");
  } else {
    // Min and max passage height
    int maxHeightThisCol;
    
    if (spaceLeft < area_height) {
      maxHeightThisCol = std::min<int>(
        // corner blocking with the left neighbor col
        area_height - oldBottomHeight - min_passage_thickness, 
        spaceLeft - min_passage_thickness
      );
    } else {
      maxHeightThisCol = area_height;
    }
    
    int minHeightThisCol = std::max<int>(spaceLeft - max_passage_thickness, 1);

    log("Top terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
    
    // generating top terrain
    log("Top steepness:");
    Vector2i topSteepness = getConstrainedSteepness(area_width - to_right_width, minHeightThisCol, maxHeightThisCol, oldTopHeight, area_height, current_stripe_number, max_top_steepness);
    
    top_terrain_height = 
      ::rand() % (topSteepness.x + topSteepness.y + 1) + oldTopHeight - topSteepness.x;
  }
  
  log("RESULT: Terrain bottom: %d, top: %d, passage: %d", bottom_terrain_height, top_terrain_height, area_height - top_terrain_height - bottom_terrain_height);
}

void PassageTurn::fillStraightPassageStripe(
  Area *area,
  int  col_index,
  int  top_terrain_height,
  int  bottom_terrain_height,
  bool horizontal)
{
  if (horizontal) {
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
      if (j < top_terrain_height || j >= area->getHeightInBlocks() - bottom_terrain_height) {
        area->add(new Terrain(), col_index, j);
      }
    }
  } else {
    col_index = area->getHeightInBlocks() - col_index - 1;
    
    for (int j = 0; j < area->getHeightInBlocks(); ++j) {    
      if (j < top_terrain_height || j >= area->getHeightInBlocks() - bottom_terrain_height) {
        area->add(new Terrain(), j, col_index);
      }
    }
  }  
}

void PassageTurn::createStraightPassage(
  Area *area,
  bool  horizontal,
  int   max_passage_thickness,
  int   min_passage_thickness,
  int   from_top_height,
  int   from_bottom_height,
  int   to_left_width,
  int   to_right_width,
  int   max_top_steepness,
  int   max_bottom_steepness)
{
  if (max_top_steepness > max_passage_thickness - min_passage_thickness
    || max_bottom_steepness > max_passage_thickness - min_passage_thickness)
  {
    throw StrugException("Steepness could not be greater than max_passage_thickness - min_passage_thickness");
  }
  
  if (max_passage_thickness - min_passage_thickness <= 0) {
    throw StrugException("Max passage height should be greater than min passage height");
  }
  
  int areaHeight = area->getHeightInBlocks();
  int areaWidth = area->getWidthInBlocks();
  int bottomTerrainHeight = from_bottom_height;
  int topTerrainHeight = from_top_height;
  
  // generating first column
  if (bottomTerrainHeight && topTerrainHeight) {
    log ("========= FIRST COL============");
    calculateStraightPassageStripe(
      bottomTerrainHeight,
      topTerrainHeight,
      to_left_width,
      to_right_width,
      areaWidth,
      areaHeight,
      0,
      max_passage_thickness,
      min_passage_thickness,
      max_top_steepness,
      max_bottom_steepness
    );
  } else {
    log("========= RANDOM FIRST COL============");
    bottomTerrainHeight = ::rand() % (areaHeight - max_passage_thickness - 1) + 1;
    int spaceLeft = areaHeight - bottomTerrainHeight;
    log("Bottom terrain: %d, space left: %d", bottomTerrainHeight, spaceLeft);
    topTerrainHeight    = ::rand() % (max_passage_thickness - min_passage_thickness) + spaceLeft - max_passage_thickness;
    log("RESULT: Bottom terrain: %d, top terrain: %d, passage: %d", bottomTerrainHeight, topTerrainHeight, areaWidth - bottomTerrainHeight - topTerrainHeight);
  }

  fillStraightPassageStripe(area, 0, topTerrainHeight, bottomTerrainHeight, horizontal);
  
  if (horizontal) {
    area->intAttributes["left_exit_top_terrain_height"]    = topTerrainHeight;
    area->intAttributes["left_exit_bottom_terrain_height"] = bottomTerrainHeight;
  } else {
    area->intAttributes["bottom_exit_left_terrain_width"]  = topTerrainHeight;
    area->intAttributes["bottom_exit_right_terrain_width"] = bottomTerrainHeight;
  }
  
  // generating the rest of the columns
  for (int i = 1; i < areaWidth; ++i) {
    log ("========= COL %d ============", i);
    calculateStraightPassageStripe(
      bottomTerrainHeight,
      topTerrainHeight,
      to_left_width,
      to_right_width,
      areaWidth,
      areaHeight,
      i,
      max_passage_thickness,
      min_passage_thickness,
      max_top_steepness,
      max_bottom_steepness
    );
    
    fillStraightPassageStripe(area, i, topTerrainHeight, bottomTerrainHeight, horizontal);
  }
  
  if (horizontal) {
    area->intAttributes["right_exit_top_terrain_height"]    = topTerrainHeight;
    area->intAttributes["right_exit_bottom_terrain_height"] = bottomTerrainHeight;
  } else {
    area->intAttributes["top_exit_left_terrain_width"]    = topTerrainHeight;
    area->intAttributes["top_exit_right_terrain_width"] = bottomTerrainHeight;
  }
}