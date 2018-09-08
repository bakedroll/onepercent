#include "simulation/ModelContainer.h"
#include "scripting/LuaModel.h"
#include "scripting/LuaStateManager.h"

#include <QMutex>

namespace onep
{
  struct ModelContainer::Impl
  {
    Impl(osgGaming::Injector& injector)
      : lua(injector.inject<LuaStateManager>())
    {}

    LuaStateManager::Ptr lua;
    LuaModel::Ptr model;

    QMutex mutexModel;
  };

  ModelContainer::ModelContainer(osgGaming::Injector& injector)
    : osg::Referenced()
    , m(new Impl(injector))
  {
  }

  ModelContainer::~ModelContainer() = default;

  void ModelContainer::initializeLuaModel()
  {
    luabridge::LuaRef refModel = m->lua->getGlobal("model");
    m->model.reset(new LuaModel(refModel));
  }

  std::shared_ptr<LuaModel> ModelContainer::getModel() const
  {
    return m->model;
  }

  void ModelContainer::accessModel(std::function<void(std::shared_ptr<LuaModel>)> func) const
  {
    QMutexLocker(&m->mutexModel);
    func(m->model);
  }
}