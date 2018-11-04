#include "core/Helper.h"

#include <QElapsedTimer>

namespace onep
{
  namespace Helper
  {
    qint64 measureMsecs(std::function<void()> task)
    {
      QElapsedTimer timer;
      timer.start();
      task();
      return timer.elapsed();
    }
  }
}