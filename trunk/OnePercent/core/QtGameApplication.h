#pragma once

#include <osgGaming/GameApplication.h>

#include <QtWidgets/QApplication>

namespace onep
{
  class QtGameApplication : public osgGaming::GameApplication
  {
  public:
    QtGameApplication(int argc, char** argv);

    void setInputManager(osg::ref_ptr<osgGaming::InputManager> im);

  protected:
    virtual int mainloop() override;
    virtual osg::ref_ptr<osgGaming::InputManager> obtainInputManager(osg::ref_ptr<osgGaming::View> view) override;

  private:
    std::shared_ptr<QApplication> m_qapplication;
    osg::ref_ptr<osgGaming::InputManager> m_inputManager;
  };
}