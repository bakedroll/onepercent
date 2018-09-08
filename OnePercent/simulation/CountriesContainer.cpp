#include "simulation/CountriesContainer.h"

#include "scripting/LuaObjectMapper.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaCountriesTable.h"

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

  std::shared_ptr<LuaCountry> CountriesContainer::getCountry(int id)
  {
    return m->countriesTable->getMappedElement<LuaCountry>(id);
  }

  void CountriesContainer::loadFromLua(const luabridge::LuaRef& object)
  {
    m->countriesTable.reset(new LuaCountriesTable(object));
  }

  void CountriesContainer::writeToLua()
  {
    m->countriesTable->traverseElementsUpdate();
  }
}