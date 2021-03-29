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
#include <QMouseEvent>

#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/PositionAttitudeTransform>

#include <QtOsgBridge/QtOsgWidget.h>
#include <QtOsgBridge/VirtualOverlay.h>
#include <QtOsgBridge/OverlayCompositor.h>

float rotation;
double position;

class EventHandler : public QObject
{
public:
  EventHandler(QMainWindow* mainWindow)
    : QObject()
    , m_mainWindow(mainWindow)
  {}

  bool eventFilter(QObject* object, QEvent* event) override
  {
    if (event->type() == QEvent::Type::MouseButtonDblClick)
    {
      const auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
      if (mouseEvent && mouseEvent->button() == Qt::MouseButton::LeftButton)
      {
        if (m_mainWindow->isFullScreen())
        {
          m_mainWindow->showNormal();
          return true;

        }
        m_mainWindow->showFullScreen();
      }
    }

    return QObject::eventFilter(object, event);
  }

private:
  QMainWindow* m_mainWindow;

};

template <typename TEffect>
osg::ref_ptr<TEffect> setupPostProcessingEffect(osgHelper::ioc::InjectionContainer& container,
                                          osgHelper::ioc::Injector& injector, const osgHelper::View::Ptr& view,
                                          QLayout* buttonsLayout, bool enable = true)
{
  container.registerType<TEffect>();
  auto effect = injector.inject<TEffect>();
  view->addPostProcessingEffect(effect, enable);

  auto button = new QPushButton(QString("Toggle %1").arg(QString::fromStdString(TEffect::Name)));
  QObject::connect(button, &QPushButton::clicked, [view]()
  {
      const auto enabled = view->getPostProcessingEffectEnabled(TEffect::Name);
      view->setPostProcessingEffectEnabled(TEffect::Name, !enabled);

      const auto message = QString("Toggled %1: %2")
                                   .arg(QString::fromStdString(TEffect::Name))
                                   .arg(enabled ? "off" : "on")
                                   .toStdString();
      printf("%s\n", message.c_str());
  });

  buttonsLayout->addWidget(button);

  return effect;
}

void createWindow(osgHelper::ioc::InjectionContainer& container, osgHelper::ioc::Injector& injector, QtOsgBridge::QtOsgWidget::UpdateMode mode)
{
  rotation = 0.0f;
  position = 0.0;

  auto geodeBox = new osg::Geode();
  geodeBox->addDrawable(new osg::ShapeDrawable(new osg::Box()));

  auto geodeSphere = new osg::Geode();
  geodeSphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));

  auto transformSphere = new osg::PositionAttitudeTransform();
  transformSphere->addChild(geodeSphere);
  transformSphere->setPosition(osg::Vec3d(2.0, 0.0, 0.0));

  auto transformBox = new osg::PositionAttitudeTransform();
  transformBox->addChild(geodeBox);
  
  auto updateTransformation = [transformBox]()
  {
    transformBox->setAttitude(osgHelper::getQuatFromEuler(0.3, 0.0, rotation));
    transformBox->setPosition(osg::Vec3d(0.0, 3.0 - 5.0 * cos(position), 0.0));
  };

  updateTransformation();

  auto group = new osg::Group();
  group->addChild(transformBox);
  group->addChild(transformSphere);

  group->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
  group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

  auto qtOsgWidget = new QtOsgBridge::QtOsgWidget();
  qtOsgWidget->setUpdateMode(mode);

  auto timer = new QTimer(qtOsgWidget);
  timer->setInterval(16);
  timer->setSingleShot(false);
  QObject::connect(timer, &QTimer::timeout, [updateTransformation]()
  {
    rotation += 0.05f;
    position += 0.02;

    if (rotation >= 2.0f * C_PI)
    {
      rotation = 0.0f;
    }

    if (position >= 2.0 * C_PI)
    {
      position = 0.0;
    }

    updateTransformation();
  });

  timer->start();

  auto sceneView  = qtOsgWidget->getView();

  auto sceneCamera = sceneView->getCamera(osgHelper::View::CameraType::Scene);

  sceneCamera->setPosition(osg::Vec3f(0, -5, 0));
  sceneCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));

  sceneView->setClampColorEnabled(true);
  sceneView->getRootGroup()->addChild(group);

  auto screenGeode = new osg::Geode();
  screenGeode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(100.0, 100.0, 0.0), osg::Vec3(200.0, 0.0, 0.0),
                                                           osg::Vec3(0.0, 100.0, 0.0)));

  auto buttonsLayout = new QHBoxLayout();
  auto fxaa          = setupPostProcessingEffect<osgHelper::ppu::FXAA>(container, injector, sceneView, buttonsLayout, false);
  auto hdr           = setupPostProcessingEffect<osgHelper::ppu::HDR>(container, injector, sceneView, buttonsLayout, false);
  auto dof           = setupPostProcessingEffect<osgHelper::ppu::DOF>(container, injector, sceneView, buttonsLayout, false);

  dof->setZNear(1.0f);
  dof->setZFar(1000.0f);
  dof->setFocalRange(3.0f);
  dof->setFocalLength(5.0f);

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

  qtOsgWidget->installEventFilter(new EventHandler(mainWindow));

  auto button1 = new QPushButton("Button 1");
  auto button2 = new QPushButton("Button 2");

  auto overlayLayout = new QVBoxLayout();
  overlayLayout->addWidget(button1);
  overlayLayout->addWidget(button2);
  overlayLayout->addStretch(1);

  auto overlay = new QtOsgBridge::VirtualOverlay();
  overlay->setLayout(overlayLayout);
  overlay->setGeometry(100, 100, 300, 300);
  overlay->setStyleSheet("background-color: rgba(100, 100, 220, 50%);");

  qtOsgWidget->getOverlayCompositor()->addVirtualOverlay(overlay);

  overlay->show();

}

int main(int argc, char** argv)
{
  osgHelper::ioc::InjectionContainer container;
  osgHelper::ioc::Injector           injector(container);

  container.registerSingletonType<osgHelper::ShaderFactory>();

  QApplication app(argc, argv);

  createWindow(container, injector, QtOsgBridge::QtOsgWidget::UpdateMode::OnTimerEvent);

  return QApplication::exec();
}
