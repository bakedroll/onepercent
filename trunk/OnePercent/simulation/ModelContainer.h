#pragma once

#include <osg/Referenced>
#include <osgGaming/Injector.h>

#include <functional>
#include <vector>

namespace onep
{
  class LuaModel;

  class ModelContainer : public osg::Referenced
  {
  public:
    using Ptr = osg::ref_ptr<ModelContainer>;

    ModelContainer(osgGaming::Injector& injector);
    ~ModelContainer();

    std::shared_ptr<LuaModel> getModel() const;

    void accessModel(std::function<void(std::shared_ptr<LuaModel>)> func) const;

    void initializeState();
    void initializeCountryNeighbours(const std::map<int, std::vector<int>>& neighbourships);
    void triggerOnInitializeEvents();

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}