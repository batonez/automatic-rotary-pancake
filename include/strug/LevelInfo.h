#pragma once

#include <string>
#include <vector>

#include <glade/util/Path.h>

struct LevelInfo
{
  Path path;
  std::string title;
};

struct LevelPackInfo
{
  std::string title;
  std::vector<LevelInfo> levels;
};
