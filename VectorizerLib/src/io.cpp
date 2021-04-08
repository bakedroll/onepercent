#include "vectorizer/io.h"

#include <fstream>
#include <stdio.h>
#include <functional>

#include <osgHelper/Helper.h>

#include <osg/Matrix>

namespace helper
{
  typedef std::vector<std::string> StringList;

  cv::Point2f getUvCoords(Graph& graph, int id)
  {
    cv::Point2f result(0.0f, 0.0f);
    if (graph.points.count(id) > 0)
    {
        const auto& point = graph.points[id];

        result.x = point.x / graph.boundary.width();
        result.y = 1.0f - (point.y / graph.boundary.height());
    }

    return result;
  }

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

  void addToRelevantVerts(IdSet& relevantIds, const Quad& quad)
  {
    for (auto i=0; i<4; i++)
    {
      relevantIds.insert(quad.idx[i]);
    }
  }

  void writeBoundariesFile(Graph& graph, BoundariesMeshData& meshdata, const char* filename)
  {
    printf("Write to boundary file: %s\n", filename);

    const auto file = fopen(filename, "w+b");
    if (!file)
    {
      return;
    }

    // Collect relevant vertices
    IdSet relevantVerts;
    for (const auto& segment : meshdata.boundarySegments)
    {
      for (const auto& quad : segment.second)
      {
        addToRelevantVerts(relevantVerts, quad);
      }
    }

    for (const auto& nodalFull : meshdata.boundaryNodalsFull)
    {
      for (const auto& quad : nodalFull.second)
      {
        addToRelevantVerts(relevantVerts, quad);
      }
    }

    for (const auto& nodal : meshdata.boundaryNodals)
    {
      for (const auto& quad : nodal.second)
      {
        addToRelevantVerts(relevantVerts, quad);
      }
    }

    // Write relevant vertices to file
    IdMap ids;
    ids[-1] = -1;
    auto i  = 0;

    writeFile<int>(file, static_cast<int>(relevantVerts.size()));
    for (const auto& vert : relevantVerts)
    {
      const auto& point = meshdata.points[vert];
      writeFile<float>(file, point.value[0]);
      writeFile<float>(file, point.value[1]);
      writeFile<float>(file, point.value[2]);
      writeFile<int>(file, ids.find(point.originId)->second);

      ids[vert] = i;
      i++;
    }

    // Write segments
    writeFile<int>(file, static_cast<int>(meshdata.boundarySegments.size()));
    for (const auto& segment : meshdata.boundarySegments)
    {
      writeFile<int>(file, segment.first);
      writeFile<int>(file, static_cast<int>(segment.second.size()));
      for (const auto& quad : segment.second)
      {
        writeFile<int>(file, ids.find(quad.idx[0])->second);
        writeFile<int>(file, ids.find(quad.idx[1])->second);
        writeFile<int>(file, ids.find(quad.idx[2])->second);
        writeFile<int>(file, ids.find(quad.idx[3])->second);
      }
    }

    // Write full nodals
    writeFile<int>(file, static_cast<int>(meshdata.boundaryNodalsFull.size()));
    for (const auto& nodalFull : meshdata.boundaryNodalsFull)
    {
      writeFile<int>(file, static_cast<int>(nodalFull.second.size()));
      for (const auto& quad : nodalFull.second)
      {
        writeFile<int>(file, ids.find(quad.idx[0])->second);
        writeFile<int>(file, ids.find(quad.idx[1])->second);
        writeFile<int>(file, ids.find(quad.idx[2])->second);
        writeFile<int>(file, ids.find(quad.idx[3])->second);
      }
    }

    // Write bordern
    writeFile<int>(file, static_cast<int>(meshdata.borders.size()));
    for (const auto& border : meshdata.borders)
    {
      writeFile<int>(file, border.first); // ID
      writeFile<int>(file, border.second.countryId);
      writeFile<int>(file, border.second.neighbourId);
      writeFile<int>(file, border.second.boundarySegmentId);
      writeFile<bool>(file, border.second.bIsCycle);

      writeFile<int>(file, static_cast<int>(border.second.nextBorderIds.size()));
      for (const auto& id : border.second.nextBorderIds)
      {
        writeFile<int>(file, id);
      }
    }

    // Write nodals
    writeFile<int>(file, static_cast<int>(meshdata.boundaryNodals.size()));
    for (const auto& nodal : meshdata.boundaryNodals)
    {
      writeFile<int>(file, nodal.first.first); // fromBorder
      writeFile<int>(file, nodal.first.second); // toBorder

      writeFile<int>(file, static_cast<int>(nodal.second.size()));
      for (const auto& quad : nodal.second)
      {
        for (int j = 0; j < 4; j++)
        {
          writeFile<int>(file, ids.find(quad.idx[j])->second);
        }
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

  void writeCountriesFile(const char* filename, Graph& graph, WorldData& world, cv::Mat& countriesMap, float shift)
  {
    printf("Write to countries file: %s\n", filename);

    const auto file = fopen(filename, "w+b");
    if (!file)
    {
      return;
    }

    auto&      countries = world.countries;
    auto&      vertices  = world.boundariesMeshData.points;
    const auto sx        = shift / graph.boundary.width();

    writeFile<int>(file, int(countries.size()));
    for (const auto& it : countries)
    {
      const auto& country = it.second;

      BoundingBox<float> bb = country.boundingbox;
      bb.shift(sx, 0.0f);

      const auto centerX = bb.center().x;
      const auto centerY = bb.center().y;

      osg::Vec2f centerLatLong((0.5f - centerY) * C_PI, fmodf(centerX + 0.5f, 1.0f) * 2.0f * C_PI);

      auto mat = osg::Matrix::identity();
      mat *= osg::Matrix::translate(osg::Vec3f(0.0f, earthRadius, 0.0f));
      mat *= osg::Matrix::rotate(osgHelper::getQuatFromEuler(centerLatLong.x(), 0.0f, centerLatLong.y()));
      mat = osg::Matrix::inverse(mat);

      // Country meta data
      writeFile<int>(file, country.data.id);
      writeFile<float>(file, centerX);
      writeFile<float>(file, centerY);
      writeFile<float>(file, bb.width(false));
      writeFile<float>(file, bb.height(false));

      // Neighbors
      writeFile<int>(file, int(country.neighbours.size()));
      for (const auto& neighbour : country.neighbours)
      {
        writeFile<int>(file, neighbour);
      }

      // Borders
      writeFile<int>(file, int(country.borders.size()));
      for (const auto& border : country.borders)
      {
        writeFile<int>(file, border.first);
        writeFile<int>(file, static_cast<int>(border.second.size()));
        for (const auto& id : border.second)
        {
          writeFile<int>(file, id);
        }
      }

      // Collect relevant vertex ids
      IdSet vertIds;
      for (const auto& triangle : country.triangles)
      {
        for (auto v=0; v<3; v++)
        {
          vertIds.insert(triangle.second.idx[v]);
        }
      }

      // build vertex array
      IdMap                   vertMap;
      std::vector<Point3D>    countryVerts;
      std::vector<osg::Vec3f> projVerts;
      osg::BoundingBox        bbProj;

      countryVerts.reserve(vertIds.size());
      projVerts.reserve(vertIds.size());

      // Preprocessing
      for (const auto& id : vertIds)
      {
        const auto& p3d = vertices[id];
        countryVerts.push_back(p3d);

        osg::Vec3f vertProj(p3d.value[0], p3d.value[1], p3d.value[2]);
        vertProj = vertProj * mat;
        projVerts.push_back(vertProj);

        bbProj.expandBy(vertProj);
      }

      const auto width  = bbProj.xMax() - bbProj.xMin();
      const auto height = bbProj.zMax() - bbProj.zMin();

      // write vertex data
      writeFile<int>(file, static_cast<int>(vertIds.size()));

      auto index=0;
      for (const auto& id : vertIds)
      {
        vertMap[id] = index;

        const auto& p3d = countryVerts[index];
        const auto  uv  = getUvCoords(graph, id);

        // Vertex
        for (auto v=0; v<3; v++)
        {
          writeFile<float>(file, p3d.value[v]);
        }

        // UV Polar
        writeFile<float>(file, uv.x);
        writeFile<float>(file, uv.y);

        // UVW Cartesian
        osg::Vec3f uvwCartesian(p3d.value[0], p3d.value[1], p3d.value[2]);
        uvwCartesian = ((uvwCartesian / earthRadius) + osg::Vec3f(1.0f, 1.0f, 1.0f)) / 2.0f;

        writeFile<float>(file, uvwCartesian.x());
        writeFile<float>(file, uvwCartesian.y());
        writeFile<float>(file, uvwCartesian.z());

        // UV Projected
        auto projVert = projVerts[index];
        osg::Vec2f projUv((projVert.x() - bbProj.xMin()) / width, (projVert.z() - bbProj.zMin()) / height);

        writeFile<float>(file, projUv.x());
        writeFile<float>(file, projUv.y());

        index++;
      }

      // Triangles
      writeFile<int>(file, static_cast<int>(country.triangles.size()));
      for (const auto& triangle : country.triangles)
      {
        for (auto v=0; v<3; v++)
        {
          writeFile<int>(file, vertMap[triangle.second.idx[v]]);
        }
      }
    }

    // Countries Map
    writeFile<int>(file, countriesMap.cols);
    writeFile<int>(file, countriesMap.rows);

    const auto xms = static_cast<int>(float(countriesMap.cols) * shift / graph.boundary.width());

    for (auto y = 0; y < countriesMap.rows; y++)
    {
      for (auto x = 0; x < countriesMap.cols; x++)
      {
        auto px = x - xms;
        while (px < 0)
        {
          px += countriesMap.cols;
        }

        writeFile<uchar>(file, countriesMap.at<uchar>(cv::Point(px % countriesMap.cols, y)));
      }
    }

    fclose(file);
  }
}