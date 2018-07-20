#include "CountriesContainer.h"

#include "scripting/LuaObjectMapper.h"
#include "simulation/Country.h"
#include "simulation/SkillsContainer.h"

#include <osgGaming/Macros.h>

namespace onep
{
  class LuaCountriesTable : public LuaObjectMapper
  {
  public:
    LuaCountriesTable(const luabridge::LuaRef& object)
      : LuaObjectMapper(object)
    {
      assert_return(object.isTable());

      for (luabridge::Iterator it(object); !it.isNil(); ++it)
      {
        luabridge::LuaRef ref = *it;
        assert_continue(ref.isTable());

        Country::Ptr country = new Country(ref);
        countries[country->getId()] = country;
      }
    }

    ~LuaCountriesTable() {}

    Country::Map countries;

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override
    {
      for (Country::Map::const_iterator it = countries.cbegin(); it != countries.cend(); ++it)
        it->second->write();
    }

    virtual void readObject(const luabridge::LuaRef& object) override {}

  };

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

  std::map<int, osg::ref_ptr<Country>>& CountriesContainer::getCountryMap() const
  {
    return m->countriesTable->countries;
  }

  osg::ref_ptr<Country> CountriesContainer::getCountry(int id)
  {
    assert_return(m->countriesTable->countries.count(id) > 0, nullptr);
    return m->countriesTable->countries[id];
  }

  void CountriesContainer::loadFromLua(const luabridge::LuaRef object)
  {
    m->countriesTable.reset(new LuaCountriesTable(object));
  }

  void CountriesContainer::writeToLua()
  {
    m->countriesTable->write();
  }
}