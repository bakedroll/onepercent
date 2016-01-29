#pragma once

#include "types.h"

namespace helper
{
  typedef struct _cycle
  {
    EdgeValueList edges;
    TriangleMap trianlges;
  } Cycle;

  typedef std::vector<Cycle> Cycles;

  void findCycles(Graph& graph, Cycles& cycles, bool debug = false, int rows = 0, int cols = 0, float scale = 1.0);
}