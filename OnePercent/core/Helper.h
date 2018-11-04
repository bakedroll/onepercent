#pragma once

#include <functional>
#include <QObject>

namespace onep
{
  namespace Helper
  {
    qint64 measureMsecs(std::function<void()> task);
  }
}