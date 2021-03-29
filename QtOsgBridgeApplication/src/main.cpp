#include <osgHelper/ShaderFactory.h>
#include <osgHelper/ioc/Injector.h>
#include <osgHelper/ppu/HDR.h>
#include <osgHelper/ppu/FXAA.h>
#include <osgHelper/ppu/DOF.h>
#include <osgHelper/Helper.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>

#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/PositionAttitudeTransform>

#include <QtOsgBridge/QtOsgWidget.h>
#include <QtOsgBridge/VirtualOverlay.h>
#include <QtOsgBridge/OverlayCompositor.h>

#include <QtOsgBridge/QtGameApplication.h>
#include <QtOsgBridge/EventStateBase.h>

float rotation;
double position;

template <typename TEffect>
void setupPostProcessingEffect(const osg::ref_ptr<TEffect>& effect, const osgHelper::View::Ptr& view,
                                                QLayout* buttonsLayout, bool enable = true)
{
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
}

class Scene : public osg::Group
{
public:
  Scene(osgHelper::ioc::Injector& injector)
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

    addChild(transformBox);
    addChild(transformSphere);

    getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
    getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    auto timer = new QTimer();
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

    auto screenGeode = new osg::Geode();
    screenGeode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(100.0, 100.0, 0.0), osg::Vec3(200.0, 0.0, 0.0),
                                                             osg::Vec3(0.0, 100.0, 0.0)));
  }
};

class InitialState : public QtOsgBridge::EventStateBase<Scene>
{
public:
  InitialState(osgHelper::ioc::Injector& injector)
    : QtOsgBridge::EventStateBase<Scene>(injector)
  {
    fxaa = injector.inject<osgHelper::ppu::FXAA>();
    dof = injector.inject<osgHelper::ppu::DOF>();
    hdr = injector.inject<osgHelper::ppu::HDR>();
  }

  void initialize(QtOsgBridge::QtOsgWidget* widget) override
  {
    auto sceneView = widget->getView();
    auto sceneCamera = sceneView->getCamera(osgHelper::View::CameraType::Scene);

    sceneCamera->setPosition(osg::Vec3f(0, -5, 0));
    sceneCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));

    sceneView->setClampColorEnabled(true);

    auto buttonsLayout = new QHBoxLayout();
    setupPostProcessingEffect<osgHelper::ppu::FXAA>(fxaa, sceneView, buttonsLayout, false);
    setupPostProcessingEffect<osgHelper::ppu::HDR>(hdr, sceneView, buttonsLayout, false);
    setupPostProcessingEffect<osgHelper::ppu::DOF>(dof, sceneView, buttonsLayout, false);

    dof->setZNear(1.0f);
    dof->setZFar(1000.0f);
    dof->setFocalRange(3.0f);
    dof->setFocalLength(5.0f);

    auto overlayLayout = new QVBoxLayout();
    overlayLayout->addLayout(buttonsLayout);
    overlayLayout->addStretch(1);

    auto overlay = new QtOsgBridge::VirtualOverlay();
    overlay->setLayout(overlayLayout);
    overlay->setGeometry(100, 100, 300, 300);
    overlay->setStyleSheet("background-color: rgba(100, 100, 220, 50%);");

    widget->getOverlayCompositor()->addVirtualOverlay(overlay);

    overlay->show();
  }

  bool isLoadingState() const override
  {
    return false;
  }

private:
  osg::ref_ptr<osgHelper::ppu::FXAA> fxaa;
  osg::ref_ptr<osgHelper::ppu::DOF> dof;
  osg::ref_ptr<osgHelper::ppu::HDR> hdr;
};

class QtOsgBridgeApplication : public QtOsgBridge::QtGameApplication
{
public:
  QtOsgBridgeApplication(int& argc, char** argv)
    : QtGameApplication(argc, argv)
  {
  }

protected:
  void registerComponents(osgHelper::ioc::InjectionContainer& container) override
  {
    registerEssentialComponents();

    container.registerType<Scene>();
    container.registerType<InitialState>();
    container.registerType<osgHelper::ppu::DOF>();
    container.registerType<osgHelper::ppu::FXAA>();
    container.registerType<osgHelper::ppu::HDR>();
  }
};

int main(int argc, char** argv)
{
  QtOsgBridgeApplication app(argc, argv);

  osgHelper::ioc::InjectionContainer container;
  osgHelper::ioc::Injector           injector(container);

  return app.runGame<InitialState>();
}
