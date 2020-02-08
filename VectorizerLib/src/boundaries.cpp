#include "vectorizer/boundaries.h"
#include "vectorizer/io.h"

#include <osgGaming/Helper.h>

namespace helper
{
  typedef struct _direction
  {
    _direction(int p1, int mid, int p2)
      : m_p1(p1)
      , m_mid(mid)
      , m_p2(p2)
    {

    }

    int m_p1;
    int m_mid;
    int m_p2;

    bool operator<(const _direction& other) const
    {
      if (m_p1 == other.m_p1)
      {
        if (m_mid == other.m_mid)
          return m_p2 < other.m_p2;

        return m_mid < other.m_mid;
      }

      return m_p1 < other.m_p1;
    }
  } Direction;

  typedef std::map<Direction, int> DirectionPointMap;

  int getPoint(IdPoint3DMap& points, DirectionPointMap& directions, int p1id, int midid, int p2id, int& id, float thickness)
  {
    DirectionPointMap::iterator it = directions.find(Direction(p1id, midid, p2id));
    if (it != directions.end())
      return it->second;

    Point3D pp1 = points.find(p1id)->second;
    Point3D pmid = points.find(midid)->second;
    Point3D pp2 = points.find(p2id)->second;

    osg::Vec3f p1 = osg::Vec3f(pp1.value[0], pp1.value[1], pp1.value[2]);
    osg::Vec3f mid = osg::Vec3f(pmid.value[0], pmid.value[1], pmid.value[2]);
    osg::Vec3f p2 = osg::Vec3f(pp2.value[0], pp2.value[1], pp2.value[2]);

    osg::Vec3f normal = mid;
    normal.normalize();

    osg::Vec3 vp1 = (p1 - mid);
    osg::Vec3 vp2 = (p2 - mid);
    vp1.normalize();
    vp2.normalize();

    float a = (3.0f - (vp1 * vp2) * C_PI) / 4.0f;

    osg::Vec3f c1 = normal ^ vp1;
    osg::Vec3f c2 = normal ^ vp2;
    c1.normalize();
    c2.normalize();

    osg::Vec3f l = c2 - c1;
    l.normalize();
    l = l * (1.0f / std::max<float>(sin(a), 0.5f));

    points.insert(IdPoint3DMap::value_type(++id, Point3D(l.x(), l.y(), l.z(), midid)));
    directions.insert(DirectionPointMap::value_type(Direction(p1id, midid, p2id), id));

    return id;
  }

  bool getPointIds(NeighbourMap& neighbours, DirectionPointMap& directions, IdPoint3DMap& points, Graph& graph, int p1id, int midid, int& l, int & r, int& id, float thickness)
  {
    NeighbourMap::iterator it = neighbours.find(midid);
    assert(it->second.size() > 1);

    int p2lid = -1;
    int p2rid = -1;
    if (it->second.size() == 2)
    {
      for (NeighbourValueList::iterator nit = it->second.begin(); nit != it->second.end(); ++nit)
      {
        if (nit->first != p1id)
        {
          p2lid = nit->first;
          p2rid = nit->first;
          break;
        }
      }
    }
    else
    {
      AnglePointIdValueMap angles;
      makeAnglePointMap(graph.points, it->second, midid, p1id, angles);

      p2lid = angles.begin()->second.first;
      p2rid = angles.rbegin()->second.first;
    }

    if (p2lid > -1 && p2rid > -1)
    {
      l = getPoint(points, directions, p1id, midid, p2lid, id, thickness);
      r = getPoint(points, directions, p2rid, midid, p1id, id, thickness);
      return true;
    }

    return false;
  }

  void makeBoundary(EdgeList& segment, Graph& graph, IdPoint3DMap& points, NeighbourMap& neighbours, DirectionPointMap& directions, QuadList& quads, IdQuadListMap& fullNodals, int& id)
  {
    for (EdgeList::iterator it = segment.begin(); it != segment.end(); ++it)
    {
      // we are at an intersection
      NeighbourValueList& neighboursp1 = neighbours[it->first];
      NeighbourValueList& neighboursp2 = neighbours[it->second];

      if (neighboursp1.size() > 2 || neighboursp2.size() > 2)
      {
        NeighbourValueList n;
        int pid;
        if (neighboursp1.size() > 2)
        {
          n = neighboursp1;
          pid = it->first;
        }
        else
        {
          n = neighboursp2;
          pid = it->second;
        }

        if (fullNodals.find(pid) != fullNodals.end())
          continue;

        QuadList fullNodal;
        for (NeighbourValueList::iterator nit = n.begin(); nit != n.end(); ++nit)
        {
          int nid = nit->first;

          int p1, p2, p3, p4;
          if (getPointIds(neighbours, directions, points, graph, pid, nid, p1, p2, id, 1.0f) &&
            getPointIds(neighbours, directions, points, graph, nid, pid, p3, p4, id, 1.0f))
          {
            fullNodal.push_back(Quad(pid, p3, p2, nid));
            fullNodal.push_back(Quad(nid, p1, p4, pid));
          }
        }

        fullNodals[pid] = fullNodal;
      }
      // regular segment
      else
      {
        int p1, p2, p3, p4;
        if (getPointIds(neighbours, directions, points, graph, it->second, it->first, p1, p2, id, 1.0f) &&
          getPointIds(neighbours, directions, points, graph, it->first, it->second, p3, p4, id, 1.0f))
        {
          quads.push_back(Quad(it->second, p3, p2, it->first));
          quads.push_back(Quad(it->first, p1, p4, it->second));
        }
      }
    }
  }

  void cleanEdges(EdgeList& edges, NeighbourMap& neighbours)
  {
    IdSet visited;

    for (NeighbourMap::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      NeighbourValueList& nlist = it->second;
      int currentId = it->first;

      if (nlist.size() == 1 && visited.count(currentId) == 0)
      {
        int lastId = currentId;

        do
        {
          visited.insert(currentId);

          for (EdgeList::iterator eit = edges.begin(); eit != edges.end(); ++eit)
          {
            if (currentId == eit->first || currentId == eit->second)
            {
              edges.erase(eit);
              break;
            }
          }

          for (NeighbourValueList::iterator nit = nlist.begin(); nit != nlist.end(); ++nit)
          {
            if (nit->first != lastId)
            {
              lastId = currentId;
              currentId = nit->first;
              break;
            }
          }

          nlist = neighbours[currentId];

        } while (nlist.size() == 2);
      }
    }
  }

  bool checkCycle(EdgeList& edges, std::set<std::pair<int, int>>& endpoints)
  {
    NeighbourMap neighbours;
    neighbourMapFromEdges(edges, neighbours);

    bool isCycle = true;
    for (NeighbourMap::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      int size = it->second.size();

      if (size > 2)
      {
        endpoints.clear();
        cleanEdges(edges, neighbours);
        return true;
      }

      if (size == 1)
      {
        std::pair<int, int> innerOuter;
        innerOuter.first = it->second[0].first;
        innerOuter.second = it->first;

        endpoints.insert(innerOuter);
        isCycle = false;
      }
    }

#ifdef _DEBUG
    assert(isCycle || endpoints.size() == 2);
#endif
    return isCycle;
  }

  void findStartAndEndPoints(
    IdPointMap& points,
    NeighbourEdgesMap& edges,
    int neighbourId,
    NeighbourMap& neighbours,
    std::set<std::pair<int, int>>& endpoints,
    std::pair<int, int>& startInnerOuterPointId,
    std::pair<int, int>& endInnerOuterPointId)
  {
    typedef std::set<std::pair<int, int>> PairSet;

    PairSet::iterator startit = endpoints.begin();
    PairSet::reverse_iterator endit = endpoints.rbegin();

    int pInner = startit->first;
    int pOuter = startit->second;
    int pInnerOther = endit->first;
    int pOuterOther = endit->second;

    NeighbourMap::iterator nit = neighbours.find(pOuter);
#ifdef _DEBUG
    assert(nit->second.size() > 2);
#endif

    AnglePointIdValueMap angles;
    makeAnglePointMap(points, nit->second, pOuter, pInner, angles);

    int nangles = angles.size();
    int i = 0;
    for (AnglePointIdValueMap::iterator ait = angles.begin(); ait != angles.end(); ++ait)
    {
      bool bIsFirst = (i == 0);
      bool bIsLast = (i == nangles-1);
      i++;

      if (!bIsFirst && !bIsLast)
        continue;

      // find matching edge point
      int pid = ait->second.first;
      for (NeighbourEdgesMap::iterator neit = edges.begin(); neit != edges.end(); ++neit)
      {
        // ignore origin egde
        if (neit->first == neighbourId)
          continue;

        for (EdgeList::iterator eit = neit->second.begin(); eit != neit->second.end(); ++eit)
        {
          if (eit->first == pid || eit->second == pid)
          {
            if (bIsFirst)
            {
              startInnerOuterPointId.first = pInner;
              startInnerOuterPointId.second = pOuter;
              endInnerOuterPointId.first = pInnerOther;
              endInnerOuterPointId.second = pOuterOther;
              return;
            }

            startInnerOuterPointId.first = pInnerOther;
            startInnerOuterPointId.second = pOuterOther;
            endInnerOuterPointId.first = pInner;
            endInnerOuterPointId.second = pOuter;
            return;
          }
        }
      }
    }
  }

  void makeNodal(Graph& graph, IdPoint3DMap& points, DirectionPointMap& directions, int first, int mid, int last, NeighbourMap neighbours, int& id, QuadList& quads)
  {
    // remove neighbours
    NeighbourMap::iterator it = neighbours.find(mid);
    NeighbourValueList::iterator nit = it->second.begin();
    while (nit != it->second.end())
    {
      if (nit->first == first || nit->first == last)
        ++nit;
      else
        nit = it->second.erase(nit);
    }

    int p1, p2, p3, p4;
    if (getPointIds(neighbours, directions, points, graph, mid, first, p1, p2, id, 1.0f) &&
      getPointIds(neighbours, directions, points, graph, first, mid, p3, p4, id, 1.0f))
    {
      quads.push_back(Quad(mid, p3, p2, first));
      quads.push_back(Quad(first, p1, p4, mid));
    }

    if (getPointIds(neighbours, directions, points, graph, last, mid, p1, p2, id, 1.0f) &&
      getPointIds(neighbours, directions, points, graph, mid, last, p3, p4, id, 1.0f))
    {
      quads.push_back(Quad(last, p3, p2, mid));
      quads.push_back(Quad(mid, p1, p4, last));
    }
  }

  void makeBoundaries(Graph& graph, CountriesMap& countries, BoundariesMeshData& boundaries, float shift)
  {
    ProgressPrinter countriesProgress("Assign countries");

    typedef std::set<int> Set;
    typedef std::pair<Set, std::set<std::pair<int, int>>> SetPair;
    typedef std::map<SetPair, int> BoundaryMap;

    int boundaryId = 0;
    int borderId = 0;

    DirectionPointMap directions;
    BoundaryMap boundaryMap;

    makeCartesianPoints(graph, boundaries.points, earthRadius, shift);

    // get current id
    int id = -1;
    for (IdPoint3DMap::iterator it = boundaries.points.begin(); it != boundaries.points.end(); ++it)
      id = std::max(id, it->first);

    NeighbourMap neighbours;
    neighbourMapFromEdges(graph.edges, neighbours);

    // for each country
    int i = 0;
    for (CountriesMap::iterator it = countries.begin(); it != countries.end(); ++it)
    {
      int countryId = it->first;
      Country& country = it->second;

      // for each neighbour
      for (NeighbourEdgesMap::iterator nit = country.neighbourEdges.begin(); nit != country.neighbourEdges.end(); ++nit)
      {
        int neighbourId = nit->first;
        EdgeList& edges = nit->second;

        Set cset;
        cset.insert(countryId);
        cset.insert(neighbourId);

        EdgeListList segments;
        findSegments(edges, segments);

        for (EdgeListList::iterator eit = segments.begin(); eit != segments.end(); ++eit)
        {
          int bid;
          std::set<std::pair<int, int>> endpoints;

          // create boundary to neighbour only if it not already exists
          bool bIsCycle = checkCycle(*eit, endpoints);
          if (bIsCycle || boundaryMap.count(SetPair(cset, endpoints)) == 0)
          {
            QuadList boundaryQuads;
            makeBoundary(*eit, graph, boundaries.points, neighbours, directions, boundaryQuads, boundaries.boundaryNodalsFull, id);

            if (boundaryQuads.size() == 0)
            {
              bid = -1;
            }
            else
            {
              bid = boundaryId++;
              boundaries.boundarySegments.insert(IdQuadListMap::value_type(bid, boundaryQuads));
            }

            boundaryMap[SetPair(cset, endpoints)] = bid;
          }
          else
          {
            bid = boundaryMap[SetPair(cset, endpoints)];
          }

          Border border;
          border.countryId = countryId;
          border.neighbourId = neighbourId;
          border.boundarySegmentId = bid;
          // border.points = ...
          border.bIsCycle = bIsCycle;

          // find endpoints
          if (!bIsCycle)
            findStartAndEndPoints(graph.points, country.neighbourEdges, nit->first, neighbours, endpoints, border.startInnerOuterPointId, border.endInnerOuterPointId);

          int boid = borderId++;

          boundaries.borders.insert(IdBorderMap::value_type(boid, border));
          country.borders[neighbourId].push_back(boid);
        }
      }

      i++;
      countriesProgress.update(i, countries.size());
    }

    // assign border ends
    i = 0;
    ProgressPrinter boundsProgress("Generate nodals");
    for (IdBorderMap::iterator it = boundaries.borders.begin(); it != boundaries.borders.end(); ++it)
    {
      i++;
      // ignore cycles
      if (it->second.bIsCycle)
      {
        boundsProgress.update(i, boundaries.borders.size());
        continue;
      }

      int pInner = it->second.endInnerOuterPointId.first;
      int pOuter = it->second.endInnerOuterPointId.second;

      IdBorderMap candidates;
      for (IdBorderMap::iterator bit = boundaries.borders.begin(); bit != boundaries.borders.end(); ++bit)
      {
        if (bit->second.startInnerOuterPointId.second == pOuter)
        {
          // exclude same border from other side
          if (it->second.countryId == bit->second.neighbourId && it->second.neighbourId == bit->second.countryId)
            continue;

          candidates.insert(IdBorderMap::value_type(bit->first, bit->second));
        }
      }

      unsigned int nassigned = 0;
      int currentNeighbourId = it->second.neighbourId;
      IdSet cassigned;
      while (nassigned < candidates.size())
      {
        IdBorderMap::iterator c1 = candidates.end();
        IdBorderMap::iterator c2 = candidates.end();

        for (IdBorderMap::iterator bit = candidates.begin(); bit != candidates.end(); ++bit)
        {
          if (cassigned.count(bit->first) > 0)
            continue;

          if (bit->second.neighbourId == currentNeighbourId)
            c1 = bit;
          else if (bit->second.neighbourId == -1)
            c2 = bit;
        }

        assert(c1 != candidates.end() || c2 != candidates.end());
        IdBorderMap::iterator cand = c1 != candidates.end() ? c1 : c2;

        it->second.nextBorderIds.push_back(cand->first);
        currentNeighbourId = cand->second.countryId;
        nassigned++;

        cassigned.insert(cand->first);

        QuadList quads;
        makeNodal(graph, boundaries.points, directions, pInner, pOuter, cand->second.startInnerOuterPointId.first, neighbours, id, quads);

        std::pair<int, int> boundDir;
        boundDir.first = it->first;
        boundDir.second = cand->first;
        boundaries.boundaryNodals[boundDir] = quads;
      }

      boundsProgress.update(i, boundaries.borders.size());
    }
  }
}