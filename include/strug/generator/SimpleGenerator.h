#pragma once

#include <strug/Level.h>

class SimpleGenerator
{
  public:
    static void fillRandom(Area *area);
    static void fillHorizontalSymmetricalPassage(Area *area, int maxPassageHeight, int minPassageHeight = 1);
    static void fillHorizontalSymmetricalPassageWithSteepness(Area *area, int maxPassageHeight, int minPassageHeight = 1, int steepness = 3);
};
