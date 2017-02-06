#include "QtGameApplication.h"

#include <osgGaming/InputManager.h>

#include <assert.h>

namespace onep
{

QtGameApplication::QtGameApplication(int argc, char** argv)
  : GameApplication()
  , m_qapplication(new QApplication(argc, argv))
{
}

void QtGameApplication::setInputManager(osg::ref_ptr<osgGaming::InputManager> im)
{
  m_inputManager = im;
}

int QtGameApplication::mainloop()
{
  getViewer()->realize();

  return m_qapplication->exec();
}

osg::ref_ptr<osgGaming::InputManager> QtGameApplication::createInputManager(osg::ref_ptr<osgGaming::View> view)
{
  assert(m_inputManager);
  return m_inputManager;
}

}