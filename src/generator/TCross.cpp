#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#include <glade/debug/log.h>
#include <glade/math/vector.h>
#include <strug/generator/TCross.h>
#include <strug/blocks/Terrain.h>
#include <strug/exception/StrugException.h>
#include <strug/generator/common.h>

void TCross::calculatePassageTurnStripe(
  /*in-out*/ int &bottom_terrain_height,
  /*in-out*/ int &top_terrain_height,
  int to_top_terrain_height,
  int to_bottom_terrain_height,
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
  
  if (current_stripe_number >= to_left_width && current_stripe_number < area_width - to_right_width) {
    bottom_terrain_height = 0;
    log("Bottom terrain height is set to zero (after turn)");
  } else {
    // Min and max passage height
    int maxHeightThisCol = area_height - oldTopHeight - min_passage_thickness;
    int minHeightThisCol = 1;
    
    // generating bottom terrain
    log("Bottom steepness:");
    Vector2i bottomSteepness;

    if (current_stripe_number < area_width - to_right_width) {
      log("Bottom terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
      bottomSteepness = get_constrained_steepness(to_left_width, minHeightThisCol, maxHeightThisCol, oldBottomHeight, 1, current_stripe_number, max_bottom_steepness);
    } else {
      // corner blocking with the right neighbor col
      if (area_width - current_stripe_number == 1 && to_bottom_terrain_height) {
        maxHeightThisCol = std::min<int>(area_height - to_top_terrain_height - min_passage_thickness, maxHeightThisCol);
      }
    
      log("Bottom terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
    
      bottomSteepness = get_constrained_steepness(area_width, minHeightThisCol, maxHeightThisCol, oldBottomHeight, to_bottom_terrain_height, current_stripe_number, max_bottom_steepness);
    }
    
    bottom_terrain_height = 
      ::rand() % (bottomSteepness.x + bottomSteepness.y + 1) + oldBottomHeight - bottomSteepness.x;
  }
  
  int spaceLeft = area_height - bottom_terrain_height;
  // the rest of the code should be completely as in straight
  log("Generateed bottom terrain: %d, Space left: %d", bottom_terrain_height, spaceLeft);
  
  // Min and max passage height
  int maxHeightThisCol;
  
  maxHeightThisCol = std::min<int>(
    // corner blocking with the left neighbor col
    area_height - oldBottomHeight - min_passage_thickness, 
    spaceLeft - min_passage_thickness - 1
  );

  // corner blocking with the right neighbor col
  if (area_width - current_stripe_number == 1 && to_top_terrain_height) {
    maxHeightThisCol = std::min<int>(area_height - to_bottom_terrain_height - min_passage_thickness, maxHeightThisCol);
  }
  
  int minHeightThisCol;
  
 // do not constrain min height for blind side of the cross, so more interesting variations are possible, and more room may be created at the cross
 // Drawback: sometimes final passage will be much taller than max passage height and it will cause big steepness leap on the edge of areas
 // if (spaceLeft >= area_height) {
    minHeightThisCol = 1; 
 // } else {
 //   minHeightThisCol = std::max<int>(spaceLeft - max_passage_thickness, 1);
 // }
  
  log("Top terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
  
  // generating top terrain
  log("Top steepness:");
  
  // as in straight
  Vector2i topSteepness = get_constrained_steepness(area_width, minHeightThisCol, maxHeightThisCol, oldTopHeight, to_top_terrain_height, current_stripe_number, max_top_steepness);
  
  top_terrain_height = 
    ::rand() % (topSteepness.x + topSteepness.y + 1) + oldTopHeight - topSteepness.x;
  
  log("RESULT: Terrain bottom: %d, top: %d, passage: %d", bottom_terrain_height, top_terrain_height, area_height - top_terrain_height - bottom_terrain_height);
}

void TCross::createPassageTurn(
  Area *area,
  bool  transpose,
  bool  invert_x,
  bool  invert_y,
  int   max_passage_thickness,
  int   min_passage_thickness,
  int   from_top_height,
  int   from_bottom_height,
  int   to_top_height,
  int   to_bottom_height,
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
  
  int bottomTerrainHeight;
  int topTerrainHeight;
  
  if (invert_y) {
    bottomTerrainHeight = from_top_height;
    topTerrainHeight = from_bottom_height;
  } else {
    bottomTerrainHeight = from_bottom_height;
    topTerrainHeight = from_top_height;
  }
  
  if (!to_left_width) {
    to_left_width = rand() % (areaWidth - max_passage_thickness - 1) + 1;
  }
  
  if (!to_right_width) {
    int spaceLeft = areaWidth - to_left_width;
    to_right_width = rand() % (max_passage_thickness - min_passage_thickness) + spaceLeft - max_passage_thickness;
  }
  
  // generating first column
  if (bottomTerrainHeight && topTerrainHeight) {
    log ("========= FIRST COL============");
    calculatePassageTurnStripe(
      bottomTerrainHeight,
      topTerrainHeight,
      to_top_height,
      to_bottom_height,
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

  fill_stripe(area, 0, topTerrainHeight, bottomTerrainHeight, transpose, invert_x, invert_y);
  
  if (!transpose && !invert_x && !invert_y) {
    area->intAttributes["left_exit_top_terrain_height"]    = topTerrainHeight;
    area->intAttributes["left_exit_bottom_terrain_height"] = bottomTerrainHeight;
  } else if (!transpose && invert_y) {
    area->intAttributes["left_exit_top_terrain_height"]    = bottomTerrainHeight;
    area->intAttributes["left_exit_bottom_terrain_height"] = topTerrainHeight;
  } else if (transpose && invert_y) {
    area->intAttributes["top_exit_right_terrain_width"]  = topTerrainHeight;
    area->intAttributes["top_exit_left_terrain_width"] = bottomTerrainHeight;
  } else if (transpose) {
    area->intAttributes["top_exit_left_terrain_width"]   = topTerrainHeight;
    area->intAttributes["top_exit_right_terrain_width"]  = bottomTerrainHeight;
  }
  
  // generating the rest of the columns
  for (int i = 1; i < areaWidth; ++i) {
    log ("========= COL %d ============", i);
    calculatePassageTurnStripe(
      bottomTerrainHeight,
      topTerrainHeight,
      to_top_height,
      to_bottom_height,
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
    
    fill_stripe(area, i, topTerrainHeight, bottomTerrainHeight, transpose, invert_x, invert_y);
  }
  
  if (!transpose && !invert_x && !invert_y) {
    area->intAttributes["bottom_exit_right_terrain_width"]  = to_right_width;
    area->intAttributes["bottom_exit_left_terrain_width"]   = to_left_width;
    area->intAttributes["right_exit_top_terrain_height"]    = topTerrainHeight;
    area->intAttributes["right_exit_bottom_terrain_height"] = bottomTerrainHeight;
  } else if (!transpose && invert_y) {
    area->intAttributes["top_exit_right_terrain_width"]     = to_right_width;
    area->intAttributes["top_exit_left_terrain_width"]      = to_left_width;
    area->intAttributes["right_exit_top_terrain_height"]    = bottomTerrainHeight;
    area->intAttributes["right_exit_bottom_terrain_height"] = topTerrainHeight;
  } else if (transpose && invert_y) {
    area->intAttributes["left_exit_top_terrain_height"]    = to_left_width;
    area->intAttributes["left_exit_bottom_terrain_height"] = to_right_width;
    area->intAttributes["bottom_exit_right_terrain_width"] = topTerrainHeight;
    area->intAttributes["bottom_exit_left_terrain_width"]  = bottomTerrainHeight;
  } else if (transpose) {
    area->intAttributes["bottom_exit_left_terrain_width"]   = topTerrainHeight;
    area->intAttributes["bottom_exit_right_terrain_width"]  = bottomTerrainHeight;
    area->intAttributes["right_exit_bottom_terrain_height"] = to_right_width;
    area->intAttributes["right_exit_top_terrain_height"]    = to_left_width;
  }
}