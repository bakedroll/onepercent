#pragma once

#include "types.h"

namespace helper
{
  void removeDuplicates(Graph& graph);
  void removeSinglePoints(Graph& graph);
  void splitSingleEdges(Graph& graph);
}