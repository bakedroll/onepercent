#pragma once

#include <QObject>

#include <functional>

namespace onep
{
  class QConnectFunctor : public QObject
  {
    Q_OBJECT

  public:
    static bool connect(
      QObject* sender,
      const char* signal,
      const std::function<void()>& receiver,
      QObject* parent = nullptr,
      Qt::ConnectionType connectionType = Qt::AutoConnection);

  private slots:
    void run();

  private:
    QConnectFunctor(QObject* parent, const std::function<void()>& func);
    QConnectFunctor(QObject* parent, const std::function<void(QObject*)>& func);

    std::function<void()> m_func;
    std::function<void(QObject*)> m_funcWithSender;

  };

  class QConnectBoolFunctor : public QObject
  {
    Q_OBJECT

  public:
    static bool connect(
      QObject* sender,
      const char* signal,
      const std::function<void(bool)>& receiver,
      QObject* parent = nullptr,
      Qt::ConnectionType connectionType = Qt::AutoConnection);

  protected slots:
    void run(bool);

  private:
    QConnectBoolFunctor(QObject* parent, const std::function<void(bool)>& func);

    std::function<void(bool)> m_func;
  };
}
