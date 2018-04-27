#include "QConnectFunctor.h"

namespace onep
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

  void QConnectBoolFunctor::run(bool i)
  {
    m_func(i);
  }

  bool QConnectBoolFunctor::connect(
    QObject* sender,
    const char* signal,
    const std::function<void(bool)>& receiver,
    QObject* parent,
    Qt::ConnectionType connectionType)
  {
    if (!parent)
      parent = sender;

    return QObject::connect(sender, signal, new QConnectBoolFunctor(parent, receiver), SLOT(run(bool)), connectionType);
  }

  QConnectBoolFunctor::QConnectBoolFunctor(QObject* parent, const std::function<void(bool)>& func)
  : QObject(parent)
  , m_func(func)
  {
    // empty
  }

  bool QConnectDoubleFunctor::connect(QObject* sender, const char* signal, const std::function<void(double)>& receiver, QObject* parent, Qt::ConnectionType connectionType)
  {
    if (!parent)
      parent = sender;

    return QObject::connect(sender, signal, new QConnectDoubleFunctor(parent, receiver), SLOT(run(double)), connectionType);
  }

  void QConnectDoubleFunctor::run(double d)
  {
    m_func(d);
  }

  QConnectDoubleFunctor::QConnectDoubleFunctor(QObject* parent, const std::function<void(double)>& func)
    : QObject(parent)
    , m_func(func)
  {
    // empty
  }
}
