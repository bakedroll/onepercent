#include <QApplication>
#include <QMainWindow>

#include <osg/ShapeDrawable>
#include <osg/Shape>

#include <osgGA/TrackballManipulator>

#include <QtOsgBridge/QtOsgWidget.h>

int main(int argc, char** argv)
{
  auto geode = new osg::Geode();
  geode->addDrawable(new osg::ShapeDrawable(new osg::Box()));

  geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
  geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

  QApplication app(argc, argv);

  auto qtOsgWidget = new QtOsgBridge::QtOsgWidget();
  qtOsgWidget->setUpdateMode(QtOsgBridge::QtOsgWidget::UpdateMode::OnTimerEvent);

  auto view = qtOsgWidget->getView();

  auto manipulator = new osgGA::TrackballManipulator();
  manipulator->setAllowThrow(false);

  view->setCameraManipulator(manipulator);
  view->setSceneData(geode);

  auto mainWindow = new QMainWindow();
  mainWindow->setGeometry(100, 100, 1024, 768);
  mainWindow->setCentralWidget(qtOsgWidget);
  mainWindow->show();

  return QApplication::exec();
}
