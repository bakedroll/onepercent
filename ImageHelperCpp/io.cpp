#include "io.h"

#include <fstream>

namespace helper
{
  void writePolyFile(Graph& graph, const char* filename)
  {
    typedef std::map<int, int> IdMap;

    IdMap ids;

    std::ofstream fs(filename);
    if (!fs.is_open())
      return;

    fs << int(graph.points.size()) << " 2 0 0" << std::endl;

    int i = 1;
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
    {
      ids.insert(IdMap::value_type(it->first, i));

      fs << i << " " << it->second.x << " " << it->second.y << std::endl;
      i++;
    }

    fs << int(graph.edges.size()) << " 1" << std::endl;

    i = 1;
    for (EdgeValueList::iterator it = graph.edges.begin(); it != graph.edges.end(); ++it)
    {
      fs << i << " " << ids[it->first.first] << " " << ids[it->first.second] << " 1" << std::endl;
      i++;
    }

    fs << 0 << std::endl;

    fs.close();
  }
}