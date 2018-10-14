#pragma once

#include <osg/Referenced>
#include <osgGaming/Injector.h>

#include <functional>

namespace onep
{
  class LuaModel;

  class ModelContainer : public osg::Referenced
  {
  public:
    using Ptr = osg::ref_ptr<ModelContainer>;

    ModelContainer(osgGaming::Injector& injector);
    ~ModelContainer();

    void initializeLuaModelData();

    std::shared_ptr<LuaModel> getModel() const;

    void accessModel(std::function<void(std::shared_ptr<LuaModel>)> func) const;

    void initializeState();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}