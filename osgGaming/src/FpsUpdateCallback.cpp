#include <osgGaming/FpsUpdateCallback.h>

#include <osg/NodeVisitor>

namespace osgGaming
{

  FpsUpdateCallback::FpsUpdateCallback()
    : osg::Callback(),
    m_framesCount(0),
    m_lastSimulationTime(0.0)
  {

  }

  void FpsUpdateCallback::setUpdateFunc(std::function<void(int)> func)
  {
    m_updateFunc = func;
  }

  bool FpsUpdateCallback::run(osg::Object* node, osg::Object* data)
  {
    if (m_updateFunc)
    {
      auto nv = dynamic_cast<osg::NodeVisitor*>(data);
      if (!nv)
      {
        return false;
      }

      double time = nv->getFrameStamp()->getSimulationTime();

      if (m_lastSimulationTime != 0.0)
      {
        if (time - m_lastSimulationTime >= 1.0)
        {
          m_updateFunc(m_framesCount);
          m_framesCount = 0;
          m_lastSimulationTime = time;
        }
        else
        {
          m_framesCount++;
        }
      }
      else
      {
        m_lastSimulationTime = time;
      }
    }

    return traverse(node, data);
  }

}