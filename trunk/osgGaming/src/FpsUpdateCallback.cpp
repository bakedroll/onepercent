#include <osgGaming/FpsUpdateCallback.h>

#include <osg/NodeVisitor>

namespace osgGaming
{

  FpsUpdateCallback::FpsUpdateCallback()
    : osg::NodeCallback(),
    m_framesCount(0),
    m_lastSimulationTime(0.0)
  {

  }

  void FpsUpdateCallback::setUpdateFunc(std::function<void(int)> func)
  {
    m_updateFunc = func;
  }

  void FpsUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    if (m_updateFunc)
    {
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

    traverse(node, nv);
  }

}