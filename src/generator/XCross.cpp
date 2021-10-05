#include <stdlib.h>
#include <algorithm>
#include <assert.h>

#include <glade/debug/log.h>
#include <glade/math/Vector.h>
#include <strug/generator/XCross.h>
#include <strug/blocks/Terrain.h>
#include <strug/exception/StrugException.h>
#include <strug/generator/common.h>

void XCross::calculatePassageTurnStripe(
  /*in-out*/ int &bottom_terrain_height,
  /*in-out*/ int &top_terrain_height,
  int to_top_terrain_height,
  int to_bottom_terrain_height,
  int to_left_width_bottommost,
  int to_right_width_bottommost,
  int to_left_width_topmost,
  int to_right_width_topmost,
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

  GENERATOR_LOG("Old terrain bottom: %d, top: %d", bottom_terrain_height, top_terrain_height);
  
  int oldBottomHeight = bottom_terrain_height;
  int oldTopHeight    = top_terrain_height;
  
  if (current_stripe_number >= to_left_width_bottommost && current_stripe_number < area_width - to_right_width_bottommost) {
    bottom_terrain_height = 0;
    GENERATOR_LOG("Bottom terrain height is set to zero (after turn)");
  } else {
    // Min and max passage height
    int maxHeightThisCol = area_height - oldTopHeight - min_passage_thickness;
    int minHeightThisCol = 1;
    
    // generating bottom terrain
    GENERATOR_LOG("Bottom steepness:");
    Vector2i bottomSteepness;

    if (current_stripe_number < area_width - to_right_width_bottommost) {
      GENERATOR_LOG("Bottom terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
      bottomSteepness = get_constrained_steepness(to_left_width_bottommost, minHeightThisCol, maxHeightThisCol, oldBottomHeight, 1, current_stripe_number, max_bottom_steepness);
    } else {
      // corner blocking with the right neighbor col
      if (area_width - current_stripe_number == 1 && to_bottom_terrain_height) {
        maxHeightThisCol = std::min<int>(area_height - to_top_terrain_height - min_passage_thickness, maxHeightThisCol);
      }
    
      GENERATOR_LOG("Bottom terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
    
      bottomSteepness = get_constrained_steepness(area_width, minHeightThisCol, maxHeightThisCol, oldBottomHeight, to_bottom_terrain_height, current_stripe_number, max_bottom_steepness);
    }
    
    bottom_terrain_height = 
      ::rand() % (bottomSteepness.x + bottomSteepness.y + 1) + oldBottomHeight - bottomSteepness.x;
  }
  
  int spaceLeft = area_height - bottom_terrain_height;
  GENERATOR_LOG("Generateed bottom terrain: %d, Space left: %d", bottom_terrain_height, spaceLeft);
  
  if (current_stripe_number >= to_left_width_topmost && current_stripe_number < area_width - to_right_width_topmost) {
    top_terrain_height = 0;
    GENERATOR_LOG("Top terrain height is set to zero (after turn)");
  } else {
    // Min and max passage height
    int maxHeightThisCol;
    
    maxHeightThisCol = std::min<int>(
      // corner blocking with the left neighbor col
      area_height - oldBottomHeight - min_passage_thickness, 
      spaceLeft - min_passage_thickness - 1
    );
    
    int minHeightThisCol = 1;
    
    // generating top terrain
    GENERATOR_LOG("Top steepness:");
    Vector2i topSteepness;
    
    if (current_stripe_number < area_width - to_right_width_topmost) {
      GENERATOR_LOG("Top terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
      topSteepness = get_constrained_steepness(to_left_width_topmost, minHeightThisCol, maxHeightThisCol, oldTopHeight, 1, current_stripe_number, max_top_steepness);
    } else {
      // corner blocking with the right neighbor col
      if (area_width - current_stripe_number == 1 && to_top_terrain_height) {
        maxHeightThisCol = std::min<int>(area_height - to_bottom_terrain_height - min_passage_thickness, maxHeightThisCol);
      }
      
      GENERATOR_LOG("Top terrain min height: %d, Max height: %d", minHeightThisCol, maxHeightThisCol);
      
      topSteepness = get_constrained_steepness(area_width, minHeightThisCol, maxHeightThisCol, oldTopHeight, to_top_terrain_height, current_stripe_number, max_top_steepness);
    }
    
    top_terrain_height = 
      ::rand() % (topSteepness.x + topSteepness.y + 1) + oldTopHeight - topSteepness.x;
  }
  
  GENERATOR_LOG("RESULT: Terrain bottom: %d, top: %d, passage: %d", bottom_terrain_height, top_terrain_height, area_height - top_terrain_height - bottom_terrain_height);
}

void XCross::createPassageTurn(
  Area *area,
  int   max_passage_thickness,
  int   min_passage_thickness,
  int   from_top_height,
  int   from_bottom_height,
  int   to_top_height,
  int   to_bottom_height,
  int   to_left_width_bottommost,
  int   to_right_width_bottommost,
  int   to_left_width_topmost,
  int   to_right_width_topmost,
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

  if (!to_left_width_bottommost) {
    to_left_width_bottommost = rand() % (areaWidth - max_passage_thickness - 1) + 1;
  }
  
  if (!to_right_width_bottommost) {
    int spaceLeft = areaWidth - to_left_width_bottommost;
    to_right_width_bottommost = rand() % (max_passage_thickness - min_passage_thickness) + spaceLeft - max_passage_thickness;
  }
  
  if (!to_left_width_topmost) {
    to_left_width_topmost = rand() % (areaWidth - max_passage_thickness - 1) + 1;
  }
  
  if (!to_right_width_topmost) {
    int spaceLeft = areaWidth - to_left_width_topmost;
    to_right_width_topmost = rand() % (max_passage_thickness - min_passage_thickness) + spaceLeft - max_passage_thickness;
  }

  if (!to_bottom_height) {
    to_bottom_height = ::rand() % (areaHeight - max_passage_thickness - 1) + 1;
    GENERATOR_LOG("Random \"to bottom\" height: %d", to_bottom_height);
  }
  
  if (!to_top_height) {
    int spaceLeft = areaHeight - to_bottom_height;
    to_top_height = ::rand() % (max_passage_thickness - min_passage_thickness) + spaceLeft - max_passage_thickness;
    GENERATOR_LOG("Random \"to top\" height: %d", to_top_height);
  }
  
  // generating first column
  if (bottomTerrainHeight && topTerrainHeight) {
    GENERATOR_LOG ("========= FIRST COL============");
    calculatePassageTurnStripe(
      bottomTerrainHeight,
      topTerrainHeight,
      to_top_height,
      to_bottom_height,
      to_left_width_bottommost,
      to_right_width_bottommost,
      to_left_width_topmost,
      to_right_width_topmost,
      areaWidth,
      areaHeight,
      0,
      max_passage_thickness,
      min_passage_thickness,
      max_top_steepness,
      max_bottom_steepness
    );
  } else {
    GENERATOR_LOG("========= RANDOM FIRST COL============");
    bottomTerrainHeight = ::rand() % (areaHeight - max_passage_thickness - 1) + 1;
    int spaceLeft = areaHeight - bottomTerrainHeight;
    GENERATOR_LOG("Bottom terrain: %d, space left: %d", bottomTerrainHeight, spaceLeft);
    topTerrainHeight    = ::rand() % (max_passage_thickness - min_passage_thickness) + spaceLeft - max_passage_thickness;
    GENERATOR_LOG("RESULT: Bottom terrain: %d, top terrain: %d, passage: %d", bottomTerrainHeight, topTerrainHeight, areaWidth - bottomTerrainHeight - topTerrainHeight);
  }

  fill_stripe(area, 0, topTerrainHeight, bottomTerrainHeight);
  
  // generating the rest of the columns
  for (int i = 1; i < areaWidth; ++i) {
    GENERATOR_LOG ("========= COL %d ============", i);
    calculatePassageTurnStripe(
      bottomTerrainHeight,
      topTerrainHeight,
      to_top_height,
      to_bottom_height,
      to_left_width_bottommost,
      to_right_width_bottommost,
      to_left_width_topmost,
      to_right_width_topmost,
      areaWidth,
      areaHeight,
      i,
      max_passage_thickness,
      min_passage_thickness,
      max_top_steepness,
      max_bottom_steepness
    );
    
    fill_stripe(area, i, topTerrainHeight, bottomTerrainHeight);
  }
}