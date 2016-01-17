#include "io.h"

#include <fstream>

namespace helper
{
  void writePolyFile(ResultPointMap& points, ResultEdgeValueList& edges, char* filename)
  {
    typedef std::map<int, int> IdMap;

    IdMap ids;

    std::ofstream fs(filename);
    if (!fs.is_open())
      return;

    fs << int(points.size()) << " 2 0 0" << std::endl;

    int i = 1;
    for (ResultPointMap::iterator it = points.begin(); it != points.end(); ++it)
    {
      ids.insert(IdMap::value_type(it->first, i));

      fs << i << " " << it->second.x << " " << it->second.y << std::endl;
      i++;
    }

    fs << int(edges.size()) << " 1" << std::endl;

    i = 1;
    for (ResultEdgeValueList::iterator it = edges.begin(); it != edges.end(); ++it)
    {
      fs << i << " " << ids[it->first.first] << " " << ids[it->first.second] << " 1" << std::endl;
      i++;
    }

    fs << 0 << std::endl;

    fs.close();
  }
}