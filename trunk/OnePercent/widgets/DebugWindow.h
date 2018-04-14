#pragma once

#include <osgGaming/Injector.h>

#include <QDialog>
#include <QLineEdit>
#include <memory>

namespace onep
{
  class ConsoleEdit : public QLineEdit
  {
    Q_OBJECT

  public:
    ConsoleEdit(QWidget* parent = nullptr);

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private:
    std::vector<QString> m_latestCommands;
    int m_pos;

  signals:
    void commandEntered(const QString& command);

  };

  class DebugWindow : public QDialog
  {
    Q_OBJECT

  public:
    DebugWindow(
      osgGaming::Injector& injector, 
      QWidget* parent = nullptr);
    ~DebugWindow();

  protected:
    void keyPressEvent(QKeyEvent* event) override;

  private slots:
    void onCommandEntered(const QString& command);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;

  };
}