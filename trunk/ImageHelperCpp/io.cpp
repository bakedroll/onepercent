#include "io.h"

#include <fstream>

namespace helper
{
  typedef std::vector<std::string> StringList;

  ProgressPrinter::ProgressPrinter(std::string name)
    : m_name(name)
    , m_percent(0)
    , m_lastUpdate(clock())
  {
    printf("%s 0%%", m_name.c_str());
  }

  void ProgressPrinter::update(long long p, long long max)
  {
    m_percent = p * 100 / max;

    if (m_percent < 0)
      printf("bla\n");

    if (clock() - m_lastUpdate >= 500 || m_percent >= 100)
    {
      printCurrent();
      m_lastUpdate = clock();
    }
  }

  void ProgressPrinter::printCurrent()
  {
    std::string p = std::to_string(m_percent) + "%";

    printf("\r%s %s", m_name.c_str(), p.c_str());

    if (m_percent >= 100)
      printf("\n");
  }

  bool readFileIntoString(const char* filename, std::string& buffer)
  {
    std::ifstream fs(filename);
    if (!fs.is_open())
      return false;

    fs.seekg(0, std::ios::end);
    buffer.reserve(size_t(fs.tellg()));
    fs.seekg(0, std::ios::beg);

    buffer.assign((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());

    fs.close();
    return true;
  }

  void writePolyFile(Graph& graph, const char* filename)
  {
    std::string fn(filename);
    fn += ".0.poly";

    typedef std::map<int, int> IdMap;

    IdMap ids;

    std::ofstream fs(fn);
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
      fs << i << " " << ids[it->first.first] << " " << ids[it->first.second] << " " << (it->second == 255 ? 0 : 1) << std::endl;
      i++;
    }

    fs << 0 << std::endl;

    fs.close();
  }

  std::string readLine(std::stringstream& stream)
  {
    char buffer[128];
    stream.getline(buffer, 255);

    return std::string(buffer);
  }

  void split(const std::string& s, char delim, StringList& elems)
  {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
      if (!item.empty())
        elems.push_back(item);
    }
  }

  void readPointElements(Graph& graph, std::stringstream& fs)
  {
    std::string headLine = readLine(fs);

    StringList elems;
    split(headLine, ' ', elems);

    int nPoints = atoi(elems[0].c_str());

    for (int i = 0; i < nPoints; i++)
    {
      std::string pointLine = readLine(fs);

      StringList pelems;
      split(pointLine, ' ', pelems);

      graph.points.insert(
        IdPointMap::value_type(
        atoi(pelems[0].c_str()),
        cv::Point2f(
        float(atof(pelems[1].c_str())),
        float(atof(pelems[2].c_str())))));
    }
  }

  void readSegmentElements(Graph& graph, std::stringstream& fs)
  {
    std::string headLine = readLine(fs);

    StringList elems;
    split(headLine, ' ', elems);

    int nEdges = atoi(elems[0].c_str());
    int bm = atoi(elems[1].c_str());

    for (int i = 0; i < nEdges; i++)
    {
      std::string edgeLine = readLine(fs);

      StringList pelems;
      split(edgeLine, ' ', pelems);

      uchar val = 255;
      if (bm == 1 && atoi(pelems[3].c_str()) == 1)
        val = 128;

      graph.edges.push_back(
        EdgeValue(
          Edge(
            atoi(pelems[1].c_str()),
            atoi(pelems[2].c_str())),
          val));
    }
  }

  void readTriangleElements(Graph& graph, std::stringstream& fs)
  {
    std::string headLine = readLine(fs);

    StringList elems;
    split(headLine, ' ', elems);

    int nTriangles = atoi(elems[0].c_str());

    for (int i = 0; i < nTriangles; i++)
    {
      std::string triLine = readLine(fs);

      StringList pelems;
      split(triLine, ' ', pelems);

      Triangle tri;
      tri.idx[0] = atoi(pelems[1].c_str());
      tri.idx[1] = atoi(pelems[2].c_str());
      tri.idx[2] = atoi(pelems[3].c_str());

      graph.triangles.insert(TriangleMap::value_type(atoi(pelems[0].c_str()), tri));
    }
  }

  void readNodeFile(Graph& graph, const char* filename)
  {
    std::string buffer;
    if (!readFileIntoString(filename, buffer))
      return;

    std::stringstream fs(buffer);
    readPointElements(graph, fs);
  }

  void readPolyFile(Graph& graph, const char* filename)
  {
    std::string buffer;
    if (!readFileIntoString(filename, buffer))
      return;

    std::stringstream fs(buffer);

    readPointElements(graph, fs);
    readSegmentElements(graph, fs);
  }

  void readEleFile(Graph& graph, const char* filename)
  {
    std::string buffer;
    if (!readFileIntoString(filename, buffer))
      return;

    std::stringstream fs(buffer);

    readTriangleElements(graph, fs);
  }

  void readGraphFiles(Graph& graph, const char* filename, int iteration)
  {
    std::string nodefn(filename);
    nodefn += "." + std::to_string(iteration) + ".node";

    std::string polyfn(filename);
    polyfn += "." + std::to_string(iteration) + ".poly";

    std::string elefn(filename);
    elefn += "." + std::to_string(iteration) + ".ele";

    printf("Read node file: %s\n", nodefn.c_str());
    readNodeFile(graph, nodefn.c_str());

    printf("Read poly file: %s\n", polyfn.c_str());
    readPolyFile(graph, polyfn.c_str());

    printf("Read ele file: %s\n", elefn.c_str());
    readEleFile(graph, elefn.c_str());
  }
}