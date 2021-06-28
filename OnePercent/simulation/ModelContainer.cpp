#include "simulation/ModelContainer.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaStateManager.h"
#include "scripting/LuaBranchesTable.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaSimulationStateTable.h"
#include "scripting/LuaValueDef.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaArrayTable.h"

#include <QMutex>

namespace onep
{
  struct ModelContainer::Impl
  {
    Impl(osgHelper::ioc::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
    {}

    LuaStateManager::Ptr lua;
    LuaModel::Ptr model;

    QRecursiveMutex mutexModel;
  };

  ModelContainer::ModelContainer(osgHelper::ioc::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
  {
    luabridge::LuaRef table = m->lua->newTable();
    m->lua->setGlobal("model", table);

    m->model = m->lua->createTableMappedObject<LuaModel>(table);
  }

  ModelContainer::~ModelContainer() = default;

  std::shared_ptr<LuaModel> ModelContainer::getModel() const
  {
    return m->model;
  }

  void ModelContainer::accessModel(std::function<void(const LuaModel::Ptr&)> func) const
  {
    QMutexLocker lock(&m->mutexModel);
    func(m->model);
  }

  void ModelContainer::initializeState()
  {
    m->lua->safeExecute([this]()
    {
      QMutexLocker lock(&m->mutexModel);

      auto stateTable = m->model->getSimulationStateTable();
      auto branchesTable = m->model->getBranchesTable();
      auto countriesTable = m->model->getCountriesTable();
      auto valuesTable = m->model->getValuesDefTable();

      countriesTable->iterateMappedObjects<LuaCountry>([&](LuaCountry::Ptr country)
      {
        auto cid = country->getId();

        stateTable->addCountryState(country->getId());
        auto state = stateTable->getCountryState(cid);

        branchesTable->iterateMappedObjects<LuaSkillBranch>([&](LuaSkillBranch::Ptr& branch)
        {
          const auto name = branch->getName();
          state->getBranchesActivatedTable()->addBranchActivated(name);
        });

        valuesTable->iterateMappedObjects<LuaValueDef>([&](LuaValueDef::Ptr& def)
        {
          const auto name   = def->getName();
          const auto group  = def->getGroup();
          const auto init   = country->getInitValue(name, def->getInit());
          auto values       = state->getValuesTable();

          if (group.empty())
          {
            values->setValue(name, init);
          }
          else
          {
            values->getGroup(group)->setValue(name, init);
          }
        });
      });

      stateTable->initializeCountryBranchActivated();
    });
  }

  void ModelContainer::initializeCountryNeighbours(const std::map<int, std::vector<int>>& neighbourships)
  {
    m->lua->safeExecute([this, &neighbourships]()
    {
      QMutexLocker lock(&m->mutexModel);

      auto statesTable = m->model->getSimulationStateTable();

      for (auto& neighbourship : neighbourships)
      {
        auto cid = neighbourship.first;
        auto nids = neighbourship.second;

        for (auto nid : nids)
        {
          luabridge::LuaRef stateRef = statesTable->getCountryState(nid)->luaRef();
          statesTable->getCountryState(cid)->addNeighbourState(nid, stateRef);
        }
      }
    });
  }
}