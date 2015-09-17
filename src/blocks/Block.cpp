#include <map>
#include <sstream>

#include <strug/blocks/Block.h>
#include <glade/math/Vector.h>
#include <glade/debug/log.h>

Block::DrawingOrder Block::drawingOrder =
  { GOLD, SLOWING_SHOT, SPEEDUP_SHOT, MUD, ROCK,
    EXIT, LADDER, PIPE, NPC, PLAYER, SPIKES, EFFECT
  };


int Block::DrawingOrderComparator::compare(GladeObject &first, GladeObject &second)
{
  int firstIndex = 0, secondIndex = 0;
  
  Block *pFirst = dynamic_cast<Block*>(&first), *pSecond = dynamic_cast<Block*>(&second);
  
  firstIndex  = getDrawingOrder(pFirst);
  secondIndex = getDrawingOrder(pSecond);
  
  return firstIndex - secondIndex;
}
  
int Block::getDrawingOrder(Block *block)
{
  if (block == NULL) {
    log("Warning: Block::getDrawingOrder, supplied block is null");
    return -1;
  }
  
  DrawingOrder::iterator i = std::find<DrawingOrder::iterator, Type>(drawingOrder.begin(), drawingOrder.end(), block->getType());
  
  if (i == drawingOrder.end()) {
    return 9999; // if there is no defined drawing order draw it last
  }
  
  return (int) (i - drawingOrder.begin());
}

Block::Block(Properties *properties):
  initialized(false)
{
  if (properties == NULL) {
    return;
  }
  
  label = (*properties)["label"]; // FIXME what if there is no label
}

void Block::initialize(const std::string &texture_pack_name, float block_width, float block_height)
{
  if (!initialized ) {
    getTransform()->setScale(block_width / 2, block_height / 2, block_width / 2);
    initialized = true;
  }
}
    
std::string Block::toString()
{
  std::ostringstream result;
  result << getType();
  
  if (label) {
    result << " label=" << label;
  }
  
  return result.str();
}


    