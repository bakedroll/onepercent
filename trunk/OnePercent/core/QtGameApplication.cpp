#include "QtGameApplication.h"

namespace onep
{
  QtGameApplication::QtGameApplication(int argc, char** argv)
    : GameApplication()
    , m_qapplication(new QApplication(argc, argv))
  {
  }

  int QtGameApplication::mainloop()
  {
    getViewer()->realize();

    return m_qapplication->exec();
  }
}