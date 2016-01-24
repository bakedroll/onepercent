#pragma once

#include "types.h"

namespace helper
{
  typedef std::vector<EdgeValueList> Cycles;

  void findCycles(Graph& graph, Cycles& cycles);
}