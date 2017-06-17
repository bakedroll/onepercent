#include "QConnectFunctor.h"

namespace osgGaming
{
  void QConnectFunctor::run()
  {
    if (m_func)
      m_func();
    else
      m_funcWithSender(sender());
  }

  bool QConnectFunctor::connect(
    QObject* sender,
    const char* signal,
    const std::function<void()>& receiver,
    QObject* parent,
    Qt::ConnectionType connectionType)
  {
    if (!parent)
      parent = sender;

    return QObject::connect(sender, signal, new QConnectFunctor(parent, receiver), SLOT(run()), connectionType);
  }

  QConnectFunctor::QConnectFunctor(QObject* parent, const std::function<void()>& func)
    : QObject(parent)
    , m_func(func)
  {
  }

  QConnectFunctor::QConnectFunctor(QObject* parent, const std::function<void(QObject*)>& func)
    : QObject(parent)
    , m_funcWithSender(func)
  {
  }
}
