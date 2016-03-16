#pragma once

#include "SimulationVisitor.h"

#include <osg/NodeCallback>

namespace onep
{
  class SimulationCallback
  {
  public:
    virtual ~SimulationCallback();
    virtual bool callback(SimulationVisitor* visitor) = 0;

  protected:
    class Callback : public osg::NodeCallback
    {
    public:
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override;
    };
  };
}