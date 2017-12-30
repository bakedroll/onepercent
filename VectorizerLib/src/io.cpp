#include "vectorizer/io.h"

#include <fstream>
#include <stdio.h>
#include <functional>

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

  template<typename T>
  void writeStream(std::ofstream& stream, T value)
  {
    stream.write(reinterpret_cast<char*>(&value), sizeof(T));
  }

  template<typename T>
  void writeFile(FILE* file, T value)
  {
    fwrite(reinterpret_cast<void*>(&value), 1, sizeof(T), file);
  }

  void writeFileString(FILE* file, std::string& str)
  {
    writeFile<int>(file, int(str.size()));
    fwrite(str.c_str(), str.size(), 1, file);
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

  // trim from start
  static inline std::string &ltrim(std::string &s)
  {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }

  // trim from end
  static inline std::string &rtrim(std::string &s)
  {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }

  // trim from both ends
  static inline std::string &trim(std::string &s)
  {
    return ltrim(rtrim(s));
  }

  void writePolyFile(Graph& graph, const char* filename)
  {
    std::string fn(filename);
    fn += ".0.poly";

    printf("Write to poly file: %s\n", fn.c_str());

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

  void writeBoundariesFile(Graph& graph, BoundariesMeshData& meshdata, const char* filename)
  {
    printf("Write to boundary file: %s\n", filename);

    FILE * file;
    file = fopen(filename, "w+b");
    if (!file)
      return;

    float width = graph.boundary.width();
    float height = graph.boundary.height();

    IdMap ids;
    ids[-1] = -1;
    int i = 0;
    int psize = int(meshdata.points.size());
    writeFile<int>(file, psize);
    for (IdPoint3DMap::iterator it = meshdata.points.begin(); it != meshdata.points.end(); ++it)
    {
      writeFile<float>(file, it->second.value[0]);
      writeFile<float>(file, it->second.value[1]);
      writeFile<float>(file, it->second.value[2]);
      writeFile<int>(file, ids.find(it->second.originId)->second);

      // texcoord
      float u = 0.0f;
      float v = 0.0f;
      if (graph.points.count(it->first) > 0)
      {
        cv::Point2f& point = graph.points[it->first];

        u = point.x / width;
        v = point.y / height;
      }

      writeFile<float>(file, u);
      writeFile<float>(file, v);

      ids.insert(IdMap::value_type(it->first, i));
      i++;
    }

    writeFile<int>(file, int(meshdata.boundarySegments.size()));
    for (IdQuadListMap::iterator it = meshdata.boundarySegments.begin(); it != meshdata.boundarySegments.end(); ++it)
    {
      writeFile<int>(file, it->first);
      writeFile<int>(file, int(it->second.size()));
      for (QuadList::iterator qit = it->second.begin(); qit != it->second.end(); ++qit)
      {
        writeFile<int>(file, ids.find(qit->idx[0])->second);
        writeFile<int>(file, ids.find(qit->idx[1])->second);
        writeFile<int>(file, ids.find(qit->idx[2])->second);
        writeFile<int>(file, ids.find(qit->idx[3])->second);
      }
    }

    writeFile<int>(file, int(meshdata.boundaryNodalsFull.size()));
    for (IdQuadListMap::iterator it = meshdata.boundaryNodalsFull.begin(); it != meshdata.boundaryNodalsFull.end(); ++it)
    {
      writeFile<int>(file, int(it->second.size()));
      for (QuadList::iterator qit = it->second.begin(); qit != it->second.end(); ++qit)
      {
        writeFile<int>(file, ids.find(qit->idx[0])->second);
        writeFile<int>(file, ids.find(qit->idx[1])->second);
        writeFile<int>(file, ids.find(qit->idx[2])->second);
        writeFile<int>(file, ids.find(qit->idx[3])->second);
      }
    }

    writeFile<int>(file, int(meshdata.borders.size()));
    for (IdBorderMap::iterator it = meshdata.borders.begin(); it != meshdata.borders.end(); ++it)
    {
      writeFile<int>(file, it->first); // ID
      writeFile<int>(file, it->second.countryId);
      writeFile<int>(file, it->second.neighbourId);
      writeFile<int>(file, it->second.boundarySegmentId);
      writeFile<bool>(file, it->second.bIsCycle);

      writeFile<int>(file, int(it->second.nextBorderIds.size()));
      for (IdList::iterator iit = it->second.nextBorderIds.begin(); iit != it->second.nextBorderIds.end(); ++iit)
        writeFile<int>(file, *iit);
    }

    writeFile<int>(file, int(meshdata.boundaryNodals.size()));
    for (BoundaryNodalMap::iterator it = meshdata.boundaryNodals.begin(); it != meshdata.boundaryNodals.end(); ++it)
    {
      writeFile<int>(file, it->first.first); // fromBorder
      writeFile<int>(file, it->first.second); // toBorder

      writeFile<int>(file, int(it->second.size()));
      for (QuadList::iterator qit = it->second.begin(); qit != it->second.end(); ++qit)
      {
        for (int j = 0; j < 4; j++)
          writeFile<int>(file, ids.find(qit->idx[j])->second);
      }
    }

    fclose(file);
  }

  std::string readLine(std::stringstream& stream)
  {
    char buffer[128];
    stream.getline(buffer, 255);

    return std::string(buffer);
  }

  void split(const std::string& s, char delim, StringList& elems, bool trimElems = false, bool ignoreEmpty = true)
  {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
      if (!ignoreEmpty || !item.empty())
      {
        //if (trimElems)
        //  item = trim(item);

        elems.push_back(item);
      }
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

  void readCountriesTable(const char* filename, CountriesTable& countries)
  {
    std::string buffer;
    if (!readFileIntoString(filename, buffer))
      return;

    // buffer = osgGaming::utf8ToLatin1(buffer.c_str());

    std::stringstream fs(buffer);

    std::string line;

    // eat first line
    std::getline(fs, line);

    while (std::getline(fs, line))
    {
      StringList elems;
      split(line, '\t', elems, true, false);
      
      if (elems.size() < 4)
        continue;

      CountryRow row;
      row.data.id = atoi(elems[0].c_str());
      row.data.name = elems[1];
      row.data.population = float(atof(elems[2].c_str()));
      row.data.gdp = float(atof(elems[3].c_str()));

      if (elems.size() >= 5)
      {
        if (elems[4][0] == '\"')
          elems[4] = std::string(elems[4].begin() + 1, elems[4].end() - 1);

        StringList cycleIds;
        split(elems[4], ';', cycleIds, true);
        for (StringList::iterator it = cycleIds.begin(); it != cycleIds.end(); ++it)
          row.cycleIds.insert(atoi(it->c_str()));
      }

      if (elems.size() >= 6)
      {
        if (elems[5][0] == '\"')
          elems[5] = std::string(elems[5].begin() + 1, elems[5].end() - 1);

        StringList anIds;
        split(elems[5], ';', anIds, true);
        for (StringList::iterator it = anIds.begin(); it != anIds.end(); ++it)
          row.additionalNeighbourIds.insert(atoi(it->c_str()));
      }

      countries.push_back(row);
    }
  }

  void writeCountriesFile(const char* filename, Graph& graph, CountriesMap& countries, cv::Mat& countriesMap, float shift)
  {
    printf("Write to countries file: %s\n", filename);

    FILE * file;
    file = fopen(filename, "w+b");
    if (!file)
      return;

    IdMap ids;
    int i = 0;
    for (IdPointMap::iterator it = graph.points.begin(); it != graph.points.end(); ++it)
    {
      ids.insert(IdMap::value_type(it->first, i));
      i++;
    }

    float sx = shift / graph.boundary.width();

    writeFile<int>(file, int(countries.size()));
    for (CountriesMap::iterator it = countries.begin(); it != countries.end(); ++it)
    {
      BoundingBox<float> bb = it->second.boundingbox;
      bb.shift(sx, 0.0f);

      writeFileString(file, it->second.data.name);
      writeFile<float>(file, it->second.data.population);
      writeFile<int>(file, int(it->second.data.gdp));
      writeFile<int>(file, it->second.data.id);
      writeFile<float>(file, bb.center().x);
      writeFile<float>(file, bb.center().y);
      writeFile<float>(file, bb.width(false));
      writeFile<float>(file, bb.height(false));

      writeFile<int>(file, int(it->second.neighbours.size()));
      for (IdSet::iterator iit = it->second.neighbours.begin(); iit != it->second.neighbours.end(); ++iit)
        writeFile<int>(file, *iit);

      writeFile<int>(file, int(it->second.borders.size()));
      for (IdBorderIdListMap::iterator nit = it->second.borders.begin(); nit != it->second.borders.end(); ++nit)
      {
        writeFile<int>(file, nit->first);
        writeFile<int>(file, int(nit->second.size()));
        for (IdList::iterator bit = nit->second.begin(); bit != nit->second.end(); ++bit)
          writeFile<int>(file, *bit);
      }

      writeFile<int>(file, int(it->second.triangles.size()));
      for (TriangleMap::iterator tit = it->second.triangles.begin(); tit != it->second.triangles.end(); ++tit)
      {
        writeFile<int>(file, ids.find(tit->second.idx[0])->second);
        writeFile<int>(file, ids.find(tit->second.idx[1])->second);
        writeFile<int>(file, ids.find(tit->second.idx[2])->second);
      }
    }

    writeFile<int>(file, countriesMap.cols);
    writeFile<int>(file, countriesMap.rows);

    int xms = int(float(countriesMap.cols) * shift / graph.boundary.width());

    for (int y = 0; y < countriesMap.rows; y++)
    {
      for (int x = 0; x < countriesMap.cols; x++)
      {
        int px = x - xms;
        while (px < 0)
          px += countriesMap.cols;

        writeFile<uchar>(file, countriesMap.at<uchar>(cv::Point(px % countriesMap.cols, y)));
      }
    }

    fclose(file);
  }
}