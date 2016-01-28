#pragma once

#include "types.h"

namespace helper
{
  typedef std::vector<EdgeValueList> Cycles;

  void findCycles(Graph& graph, Cycles& cycles, bool debug = false, int rows = 0, int cols = 0, float scale = 1.0);
}