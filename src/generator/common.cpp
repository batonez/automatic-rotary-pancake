#include <algorithm>

#include <glade/debug/log.h>

#include <strug/generator/common.h>
#include <strug/blocks/Terrain.h>

int not_negative(int val)
{
  return val < 0 ? 0 : val;
}

void fill_stripe(
  Area *area,
  int  col_index,
  int  top_terrain_height,
  int  bottom_terrain_height,
  bool transpose,
  bool invert_x,
  bool invert_y)
{

  if (invert_x) {
    col_index = area->getWidthInBlocks() - col_index - 1;
  }
  
  register int j, jsave;
  int &firstCoord  = transpose ? j : col_index;
  int &secondCoord = transpose ? col_index : j;
    
  for (j = 0; j < area->getHeightInBlocks(); ++j) {    
    if (j < top_terrain_height || j >= area->getHeightInBlocks() - bottom_terrain_height) {
      jsave = j;
      
      if (invert_y) {  
        j = area->getHeightInBlocks() - j - 1;
      }
      
      area->add(new Terrain(), firstCoord, secondCoord);
      j = jsave;
    }
  }
}


Vector2i get_constrained_steepness(
  int  area_width,
  int  min_height_this_col,
  int  max_height_this_col,
  int  from_height,
  int  to_height,
  int  current_stripe_number,
  int  max_steepness,
  bool exact_destination)
{
  Vector2i result(max_steepness, max_steepness);
  log("S %d %d", result.x, result.y);
  
  // Cannot let the passage be taller than max passage height  
  result.x = std::min<int>(not_negative(from_height - min_height_this_col), result.x);  
  // Exclude corner blocking
  result.y = std::min<int>(not_negative(max_height_this_col - from_height), result.y);
  
  log("S %d %d", result.x, result.y);
  
  if (!from_height || !to_height) {
    return result;
  }
  
  int colsLeft = area_width - current_stripe_number;
  int maxFinalHeight = from_height + colsLeft * max_steepness;
  int minFinalHeight = from_height - colsLeft * max_steepness;
  int maxRequiredFinalHeight = to_height;
  int minRequiredFinalHeight = to_height;
  
  if (!exact_destination) {
    maxRequiredFinalHeight += max_steepness;
    minRequiredFinalHeight -= max_steepness;
  }
  
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