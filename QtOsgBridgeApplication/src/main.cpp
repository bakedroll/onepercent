#include <QApplication>
#include <QMainWindow>

#include <osg/ShapeDrawable>
#include <osg/Shape>

#include <QtOsgBridge/QtOsgWidget.h>

void createWindow(const osg::ref_ptr<osg::Geode>& geode, QtOsgBridge::QtOsgWidget::UpdateMode mode)
{
  /*auto geode2 = new osg::Geode();
  geode2->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));

  geode2->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
  geode2->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  geode2->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ONE), osg::StateAttribute::ON);*/
  

  auto qtOsgWidget = new QtOsgBridge::QtOsgWidget();
  qtOsgWidget->setUpdateMode(mode);

  auto sceneView  = qtOsgWidget->getView(QtOsgBridge::QtOsgWidget::ViewType::Scene);
  auto screenView = qtOsgWidget->getView(QtOsgBridge::QtOsgWidget::ViewType::Screen);

  auto sceneCamera  = qtOsgWidget->getCamera(QtOsgBridge::QtOsgWidget::ViewType::Scene);
  auto screenCamera = qtOsgWidget->getCamera(QtOsgBridge::QtOsgWidget::ViewType::Screen);

  sceneCamera->setPosition(osg::Vec3f(0, -5, 0));
  sceneCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));

  screenCamera->setPosition(osg::Vec3f(1, -12, 0));
  sceneView->setClampColorEnabled(true);
  sceneView->getRootGroup()->addChild(geode);

  auto screenGeode = new osg::Geode();
  screenGeode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(100.0, 100.0, 0.0), osg::Vec3(200.0, 0.0, 0.0), osg::Vec3(0.0, 100.0, 0.0)));

  screenView->getRootGroup()->addChild(screenGeode);

  auto mainWindow = new QMainWindow();
  mainWindow->setGeometry(100, 100, 1024, 768);
  mainWindow->setCentralWidget(qtOsgWidget);
  mainWindow->show();
}

int main(int argc, char** argv)
{
  auto geode = new osg::Geode();
  geode->addDrawable(new osg::ShapeDrawable(new osg::Box()));

  geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
  geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

  QApplication app(argc, argv);

  createWindow(geode, QtOsgBridge::QtOsgWidget::UpdateMode::OnTimerEvent);

  return QApplication::exec();
}
