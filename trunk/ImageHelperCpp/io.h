#pragma once

#include "types.h"

namespace helper
{
  void writePolyFile(Graph& graph, const char* filename);
  void readGraphFiles(Graph& graph, const char* filename, int iteration);
}