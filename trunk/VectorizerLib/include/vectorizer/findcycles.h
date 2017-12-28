#pragma once

#include "types.h"

namespace helper
{
  void findCycles(Graph& graph, CyclesMap& cycles, bool debug = false, float scale = 1.0);
}