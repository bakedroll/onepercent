#pragma once

#include <osgGaming/GameApplication.h>

#include <QtWidgets/QApplication>

namespace onep
{
  class QtGameApplication : public osgGaming::GameApplication
  {
  public:
    QtGameApplication(int argc, char** argv);

  protected:
    virtual int mainloop() override;

  private:
    std::shared_ptr<QApplication> m_qapplication;
  };
}