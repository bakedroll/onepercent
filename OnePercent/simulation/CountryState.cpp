#include "simulation/CountryState.h"
#include "simulation/SkillBranch.h"
#include "core/Macros.h"
#include "core/Multithreading.h"

namespace onep
{
  typedef std::map<std::string, osgGaming::Observable<bool>::Ptr> BranchActivatedMap;

  class ValuesTable : public LuaObjectMapper
  {
  public:
    ValuesTable(const luabridge::LuaRef& object)
      : LuaObjectMapper(object)
    {
      readObject(object);
    }

    ~ValuesTable() {}

    CountryState::ValuesMap values;

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override
    {
      for (CountryState::ValuesMap::const_iterator it = values.cbegin(); it != values.cend(); ++it)
        object[it->first.c_str()] = it->second;
    }

    virtual void readObject(const luabridge::LuaRef& object) override
    {
      for (luabridge::Iterator it(object); !it.isNil(); ++it)
      {
        assert_continue((*it).isNumber());
        assert_continue(it.key().isString());

        values[it.key()] = float(*it);
      }
    }

  };

  class BranchValuesTable : public LuaObjectMapper
  {
  public:
    BranchValuesTable(const luabridge::LuaRef& object)
      : LuaObjectMapper(object)
    {
      readObject(object);
    }

    ~BranchValuesTable() {}

    CountryState::BranchValuesMap branchValues;

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override
    {
      for (CountryState::BranchValuesMap::const_iterator it = branchValues.cbegin(); it != branchValues.cend(); ++it)
      {
        luabridge::LuaRef refValues = object[it->first];
        for (CountryState::ValuesMap::const_iterator vit = it->second.cbegin(); vit != it->second.cend(); ++vit)
          refValues[vit->first] = vit->second;
      }
    }

    virtual void readObject(const luabridge::LuaRef& object) override
    {
      for (luabridge::Iterator it(object); !it.isNil(); ++it)
      {
        assert_continue((*it).isTable());
        assert_continue(it.key().isString());

        std::string branchName = it.key().tostring();

        for (luabridge::Iterator vit(*it); !vit.isNil(); ++vit)
        {
          assert_continue((*vit).isNumber());
          assert_continue(vit.key().isString());

          branchValues[branchName][vit.key()] = float(*vit);
        }
      }
    }

  };

  class BranchesActivatedTable : public LuaObjectMapper
  {
  public:
    BranchesActivatedTable(const luabridge::LuaRef& object)
      : LuaObjectMapper(object)
    {
      for (luabridge::Iterator it(object); !it.isNil(); ++it)
      {
        assert_continue((*it).type() == LUA_TBOOLEAN);
        assert_continue(it.key().isString());

        oActivatedBranches[it.key()] = new osgGaming::Observable<bool>(bool(*it));
      }
    }

    ~BranchesActivatedTable() {}

    BranchActivatedMap oActivatedBranches;

  protected:
    virtual void writeObject(luabridge::LuaRef& object) const override
    {
      for (BranchActivatedMap::const_iterator it = oActivatedBranches.cbegin(); it != oActivatedBranches.cend(); ++it)
        object[it->first] = it->second->get();
    }

    virtual void readObject(const luabridge::LuaRef& object) override
    {
      for (luabridge::Iterator it(object); !it.isNil(); ++it)
      {
        assert_continue(it.key().isString());
        osgGaming::Observable<bool>::Ptr oActivated = oActivatedBranches[it.key()];

        assert_continue((*it).type() == LUA_TBOOLEAN);
        assert_continue(oActivated.valid());

        if (oActivated->get() != bool(*it))
          oActivated->set(bool(*it));
      }
    }

  };

  struct CountryState::Impl
  {
    Impl() {}

    std::shared_ptr<ValuesTable> tValues;
    std::shared_ptr<BranchValuesTable> tBranchValues;
    std::shared_ptr<BranchesActivatedTable> tBranchesActivated;

    QMutex mutexActivated;
  };

  CountryState::CountryState(const luabridge::LuaRef& object)
    : osg::Referenced()
    , LuaObjectMapper(object)
    , m(new Impl())
  {
    luabridge::LuaRef refValues = object["values"];
    luabridge::LuaRef refBranchValues = object["branch_values"];
    luabridge::LuaRef refBranchesActivated = object["branches_activated"];

    assert_return(refValues.isTable());
    assert_return(refBranchValues.isTable());
    assert_return(refBranchesActivated.isTable());

    m->tValues.reset(new ValuesTable(refValues));
    m->tBranchValues.reset(new BranchValuesTable(refBranchValues));
    m->tBranchesActivated.reset(new BranchesActivatedTable(refBranchesActivated));
  }

  CountryState::~CountryState()
  {
  }

  CountryState::ValuesMap& CountryState::getValuesMap() const
  {
    return m->tValues->values;
  }

  CountryState::BranchValuesMap& CountryState::getBranchValuesMap() const
  {
    return m->tBranchValues->branchValues;
  }

  bool CountryState::getBranchActivated(const char* branchName) const
  {
    QMutexLocker lock(&m->mutexActivated);
    return getOActivatedBranch(branchName)->get();
  }

  void CountryState::setBranchActivated(const char* branchName, bool activated)
  {
    QMutexLocker lock(&m->mutexActivated);
    getOActivatedBranch(branchName)->set(activated);
  }

  osgGaming::Observable<bool>::Ptr CountryState::getOActivatedBranch(const char* branchName) const
  {
    std::string branchNameStr(branchName);

    if (m->tBranchesActivated->oActivatedBranches.count(branchNameStr) == 0)
      m->tBranchesActivated->oActivatedBranches.insert(BranchActivatedMap::value_type(branchNameStr, new osgGaming::Observable<bool>(false)));

    return m->tBranchesActivated->oActivatedBranches[branchNameStr];
  }

  void CountryState::writeValues()
  {
    m->tValues->write();
  }

  void CountryState::writeBranchValues()
  {
    m->tBranchValues->write();
  }

  void CountryState::writeBranchesActivated()
  {
    m->tBranchesActivated->write();
  }

  void CountryState::writeObject(luabridge::LuaRef& object) const
  {
    m->tValues->write();
    m->tBranchValues->write();
    m->tBranchesActivated->write();
  }

  void CountryState::readObject(const luabridge::LuaRef& object)
  {
    m->tValues->read();
    m->tBranchValues->read();
    m->tBranchesActivated->read();
  }
}