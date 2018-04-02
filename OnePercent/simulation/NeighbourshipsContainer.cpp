#include "NeighbourshipsContainer.h"

#include "core/Macros.h"
#include "nodes/CountryOverlay.h"
#include "simulation/Neighbourship.h"
#include "simulation/SimulatedValuesContainer.h"

#include <QString>

namespace onep
{
  struct NeighbourshipsContainer::Impl
  {
    Impl(osgGaming::Injector& injector)
      : countryOverlay(injector.inject<CountryOverlay>())
      , simulatedValuesContainer(injector.inject<SimulatedValuesContainer>())
    {}

    CountryOverlay::Ptr countryOverlay;
    SimulatedValuesContainer::Ptr simulatedValuesContainer;

    std::map<int, Neighbourship::Ptr> neighbourships;
  };

  NeighbourshipsContainer::NeighbourshipsContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
  {
  }

  NeighbourshipsContainer::~NeighbourshipsContainer()
  {
  }

  void NeighbourshipsContainer::registerClass(lua_State* state)
  {
    luabridge::getGlobalNamespace(state)
      .beginClass<NeighbourshipsContainer>("NeighbourshipsContainer")
      .addFunction("get_neighbourship", &NeighbourshipsContainer::lua_get_neighbourship)
      .endClass();
  }

  std::string NeighbourshipsContainer::instanceVariableName()
  {
    return "neighbourshipsContainer";
  }

  void NeighbourshipsContainer::prepare()
  {
    CountryOverlay::NeighbourMap& neighbourships = m->countryOverlay->getNeighbourships();
    CountryState::Map& countryStates = m->simulatedValuesContainer->getState()->getCountryStates();

    for (CountryOverlay::NeighbourMap::iterator it = neighbourships.begin(); it != neighbourships.end(); ++it)
    {
      Neighbourship::Ptr neighbourship = new Neighbourship();

      for (std::vector<int>::iterator iit = it->second.begin(); iit != it->second.end(); ++iit)
      {
        if (countryStates.count(*iit) == 0)
        {
          OSGG_QLOG_WARN(QString("CountryState for country with id %1 not found").arg(*iit));
          continue;
        }

        neighbourship->addNeighbour(countryStates[*iit].get());
      }

      m->neighbourships[it->first] = neighbourship;
    }
  }

  Neighbourship* NeighbourshipsContainer::lua_get_neighbourship(int country_id)
  {
    return m->neighbourships[country_id].get();
  }
}