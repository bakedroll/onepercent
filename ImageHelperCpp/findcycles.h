#pragma once

#include "types.h"

namespace helper
{
  typedef struct _cycle
  {
    EdgeValueList edges;
    TriangleMap trianlges;
    BoundingBox<float> boundingbox;
  } Cycle;

  typedef std::vector<Cycle> Cycles;

  void findCycles(Graph& graph, Cycles& cycles, bool debug = false, float scale = 1.0);
}