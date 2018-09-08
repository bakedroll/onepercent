#include "CountriesContainer.h"

#include "scripting/LuaObjectMapper.h"
#include "simulation/Country.h"
#include "simulation/LuaCountriesTable.h"

namespace onep
{
  struct CountriesContainer::Impl
  {
    Impl() {}

    std::shared_ptr<LuaCountriesTable> countriesTable;
  };

  CountriesContainer::CountriesContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl())
  {
  }

  CountriesContainer::~CountriesContainer()
  {
  }

  std::shared_ptr<Country> CountriesContainer::getCountry(int id)
  {
    return m->countriesTable->getMappedElement<Country>(id);
  }

  void CountriesContainer::loadFromLua(const luabridge::LuaRef object)
  {
    m->countriesTable.reset(new LuaCountriesTable(object));
  }

  void CountriesContainer::writeToLua()
  {
    m->countriesTable->traverseElementsUpdate();
  }
}