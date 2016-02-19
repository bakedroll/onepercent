#include "countries.h"
#include "draw.h"

namespace helper
{
  void makeCountries(Graph& graph, CyclesMap& cycles, CountriesTable& table, float mapscale, CountriesMap& countries, cv::Mat& countriesMap)
  {
    countriesMap = cv::Mat(int(graph.boundary.height() * mapscale), int(graph.boundary.width() * mapscale), CV_8U);
    countriesMap.setTo(0);

    IdMap cycleCountryMap;
    IdIdMap countryCycleNeighboursMap;
    for (CountriesTable::iterator it = table.begin(); it != table.end(); ++it)
    {
      // add only countries which have cycleIds defined
      if (it->cycleIds.empty())
        continue;

      Country country;
      country.data = it->data;

      IdSet neighboursSet;

      // add triangles
      for (IdSet::iterator iit = it->cycleIds.begin(); iit != it->cycleIds.end(); ++iit)
      {
        CyclesMap::iterator cit = cycles.find(*iit);
        if (cit == cycles.end() || cycleCountryMap.find(*iit) != cycleCountryMap.end())
          continue;

        country.boundingbox.expand(cit->second.boundingbox);
        for (TriangleMap::iterator tit = cit->second.trianlges.begin(); tit != cit->second.trianlges.end(); ++tit)
        {
          country.triangles.insert(TriangleMap::value_type(tit->first, tit->second));
          drawFilledTriangle(countriesMap, graph, tit->second.idx, mapscale, uchar(country.data.id));
        }

        cycleCountryMap.insert(IdMap::value_type(*iit, country.data.id));
        for (NeighbourEdgesMap::iterator eit = cit->second.edges.begin(); eit != cit->second.edges.end(); ++eit)
        {
          if (eit->first < 0 || neighboursSet.find(eit->first) != neighboursSet.end() || it->cycleIds.find(eit->first) != it->cycleIds.end())
            continue;

          neighboursSet.insert(eit->first);
        }
      }

      if (country.triangles.empty())
        continue;

      country.boundingbox.scale(1.0f / graph.boundary.width(), 1.0f / graph.boundary.height());

      countries.insert(CountriesMap::value_type(country.data.id, country));
      countryCycleNeighboursMap.insert(IdIdMap::value_type(country.data.id, neighboursSet));
    }

    for (CountriesTable::iterator it = table.begin(); it != table.end(); ++it)
    {
      CountriesMap::iterator cit = countries.find(it->data.id);
      if (cit == countries.end())
        continue;

      for (IdSet::iterator iit = it->additionalNeighbourIds.begin(); iit != it->additionalNeighbourIds.end(); ++iit)
      {
        CountriesMap::iterator nit = countries.find(*iit);

        if (cit->second.neighbours.find(*iit) != cit->second.neighbours.end() ||
          nit == countries.end())
          continue;

        cit->second.neighbours.insert(*iit);

        if (nit->second.neighbours.find(*iit) != nit->second.neighbours.end())
          continue;

        nit->second.neighbours.insert(cit->first);
      }
    }

    for (CountriesMap::iterator it = countries.begin(); it != countries.end(); ++it)
    {
      IdIdMap::iterator ccit = countryCycleNeighboursMap.find(it->first);

      for (IdSet::iterator iit = ccit->second.begin(); iit != ccit->second.end(); ++iit)
      {
        IdMap::iterator idit = cycleCountryMap.find(*iit);
        if (idit == cycleCountryMap.end() ||
          it->second.neighbours.find(idit->second) != it->second.neighbours.end() ||
          countryCycleNeighboursMap.find(idit->second) == countryCycleNeighboursMap.end())
          continue;

        it->second.neighbours.insert(idit->second);
      }
    }
  }
}