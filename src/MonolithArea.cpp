#include <glade/debug/log.h>
#include <glade/math/Vector.h>
#include <strug/exception/StrugException.h>
#include <strug/MonolithArea.h>
#include <strug/blocks/Terrain.h>

MonolithArea::MonolithArea(int width_param, int height_param):
  Area(width_param, height_param)
{}

MonolithArea::~MonolithArea() {}

MonolithArea::BlockTypes* MonolithArea::getObjectTypesAt(int x, int y)
{
  return &outOfBoundsBlockTypes;
}

MonolithArea::Blocks* MonolithArea::getObjectsAt(int x, int y)
{
  return &outOfBoundsCell;
}

Terrain* MonolithArea::getTerrainAt(int x, int y)
{
  return dynamic_cast<Terrain*>(outOfBoundsCell.at(0));
}

Block* MonolithArea::getBlockByLabel(int label)
{
  return outOfBoundsCell.at(0);
}

bool MonolithArea::isThereAnObjectOfType(Block::Type typeId, int blockX, int blockY)
{
  return typeId == Block::TERRAIN;
}

void MonolithArea::add(Block *object, int blockX, int blockY)
{
  log("Warning: MonolithArea::add() not implemented");
}

void MonolithArea::remove(int blockX, int blockY)
{
  log("Warning: MonolithArea::add() not implemented");
}