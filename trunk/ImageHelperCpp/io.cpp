#include "io.h"

#include <fstream>

namespace helper
{
  void writePolyFile(ResultPointMap& points, ResultEdgeValueList& edges, char* filename)
  {
    std::ofstream fs(filename);
    if (!fs.is_open())
      return;

    fs << int(points.size()) << " 2 0 0" << std::endl;

    for (ResultPointMap::iterator it = points.begin(); it != points.end(); ++it)
      fs << it->first << " " << it->second.x << " " << it->second.y << std::endl;

    fs << int(edges.size()) << " 1" << std::endl;

    int i = 1;
    for (ResultEdgeValueList::iterator it = edges.begin(); it != edges.end(); ++it)
    {
      fs << i << " " << it->first.first << " " << it->first.second << " 1" << std::endl;
      i++;
    }

    fs << 0 << std::endl;

    fs.close();
  }
}