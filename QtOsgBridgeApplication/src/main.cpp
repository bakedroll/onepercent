#include <osgHelper/ShaderFactory.h>
#include <osgHelper/ioc/Injector.h>
#include <osgHelper/ppu/HDR.h>
#include <osgHelper/ppu/FXAA.h>
#include <osgHelper/ppu/DOF.h>
#include <osgHelper/Helper.h>

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>

#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/PositionAttitudeTransform>

#include <QtOsgBridge/QtOsgWidget.h>

template <typename T>
osg::ref_ptr<T> setupPostProcessingEffect(osgHelper::ioc::InjectionContainer& container,
                                          osgHelper::ioc::Injector& injector, const osgHelper::View::Ptr& view,
                                          QLayout* buttonsLayout, bool enable = true)
{
  container.registerType<T>();
  auto effect = injector.inject<T>();
  view->addPostProcessingEffect(effect, enable);

  auto button = new QPushButton(QString("Toggle %1").arg(QString::fromStdString(T::Name)));
  QObject::connect(button, &QPushButton::clicked, [view]()
  {
      const auto enabled = view->getPostProcessingEffectEnabled(T::Name);
      view->setPostProcessingEffectEnabled(T::Name, !enabled);

      const auto message = QString("Toggled %1: %2\n")
                                   .arg(QString::fromStdString(T::Name))
                                   .arg(enabled ? "off" : "on")
                                   .toStdString();
      printf(message.c_str());
  });

  buttonsLayout->addWidget(button);

  return effect;
}

void createWindow(osgHelper::ioc::InjectionContainer& container, osgHelper::ioc::Injector& injector, QtOsgBridge::QtOsgWidget::UpdateMode mode)
{
  auto geodeBox = new osg::Geode();
  geodeBox->addDrawable(new osg::ShapeDrawable(new osg::Box()));

  auto geodeSphere = new osg::Geode();
  geodeSphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));

  auto transformSphere = new osg::PositionAttitudeTransform();
  transformSphere->addChild(geodeSphere);
  transformSphere->setPosition(osg::Vec3d(2.0, 0.0, 0.0));

  auto transformBox = new osg::PositionAttitudeTransform();
  transformBox->addChild(geodeBox);
  transformBox->setAttitude(osgHelper::getQuatFromEuler(0.3, 0.0, 0.4));

  auto group = new osg::Group();
  group->addChild(transformBox);
  group->addChild(transformSphere);

  group->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
  group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

  auto qtOsgWidget = new QtOsgBridge::QtOsgWidget();
  qtOsgWidget->setUpdateMode(mode);

  auto sceneView  = qtOsgWidget->getView(QtOsgBridge::QtOsgWidget::ViewType::Scene);
  auto screenView = qtOsgWidget->getView(QtOsgBridge::QtOsgWidget::ViewType::Screen);

  auto sceneCamera  = qtOsgWidget->getCamera(QtOsgBridge::QtOsgWidget::ViewType::Scene);
  auto screenCamera = qtOsgWidget->getCamera(QtOsgBridge::QtOsgWidget::ViewType::Screen);

  sceneCamera->setPosition(osg::Vec3f(0, -5, 0));
  sceneCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));

  screenCamera->setPosition(osg::Vec3f(1, -12, 0));
  sceneView->getRootGroup()->addChild(group);

  auto screenGeode = new osg::Geode();
  screenGeode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(100.0, 100.0, 0.0), osg::Vec3(200.0, 0.0, 0.0),
                                                           osg::Vec3(0.0, 100.0, 0.0)));

  screenView->getRootGroup()->addChild(screenGeode);

  auto buttonsLayout = new QHBoxLayout();
  auto fxaa          = setupPostProcessingEffect<osgHelper::ppu::FXAA>(container, injector, sceneView, buttonsLayout, false);
  auto hdr           = setupPostProcessingEffect<osgHelper::ppu::HDR>(container, injector, sceneView, buttonsLayout, false);
  auto dof           = setupPostProcessingEffect<osgHelper::ppu::DOF>(container, injector, sceneView, buttonsLayout, false);

  dof->setZNear(sceneCamera->getProjectionNear());
  dof->setZFar(sceneCamera->getProjectionFar());
  dof->setFocalRange(3.0f);
  dof->setFocalLength(1.0f);
  dof->setGaussRadius(3.5f);

  auto mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(qtOsgWidget);
  mainLayout->addLayout(buttonsLayout);

  auto widget = new QWidget();
  widget->setLayout(mainLayout);

  auto mainWindow = new QMainWindow();
  mainWindow->setGeometry(100, 100, 1024, 768);
  mainWindow->setCentralWidget(widget);
  mainWindow->show();
}

int main(int argc, char** argv)
{
  osgHelper::ioc::InjectionContainer container;
  osgHelper::ioc::Injector           injector(container);

  container.registerSingletonType<osgHelper::ShaderFactory>();


  QSurfaceFormat format;
  //format.setVersion(2, 1);
  //format.setProfile( QSurfaceFormat::CompatibilityProfile );

  format.setRedBufferSize(16);
  format.setGreenBufferSize(16);
  format.setBlueBufferSize(16);
  format.setAlphaBufferSize(16);
  format.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);

  QSurfaceFormat::setDefaultFormat(format);


  QApplication app(argc, argv);

  createWindow(container, injector, QtOsgBridge::QtOsgWidget::UpdateMode::OnTimerEvent);

  return QApplication::exec();
}
