#include "SimulationCallback.h"

namespace onep
{
  SimulationCallback::~SimulationCallback()
  {
    
  }

  void SimulationCallback::Callback::operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    SimulationVisitor* visitor = dynamic_cast<SimulationVisitor*>(nv);
    if (!visitor)
      return;

    SimulationCallback* c = dynamic_cast<SimulationCallback*>(node);
    if (c->callback(visitor))
      traverse(node, nv);
  }
}