#include "InitialState.h"

#include <QtOsgBridge/VirtualOverlay.h>
#include <QtOsgBridge/OverlayCompositor.h>

#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/PositionAttitudeTransform>

#include <osgHelper/Helper.h>

#include <QVBoxLayout>
#include <QPushButton>

#include <QMouseEvent>

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

State1::State1(osgHelper::ioc::Injector& injector)
  : QtOsgBridge::EventProcessingState(injector)
{
}

State1::~State1()
{
  OSGG_LOG_DEBUG("~destruct State1");
}

void State1::onInitialize(QtOsgBridge::MainWindow* mainWindow)
{
  OSGG_LOG_DEBUG("onInitialize() State1");

  const auto b1 = new QPushButton("State 2");
  const auto b2 = new QPushButton("State 2 Exit");
  const auto b3 = new QPushButton("Close");

  connect(b1, &QPushButton::clicked, [this]()
  {
    requestNewEventState<State2>(NewEventStateMode::ContinueCurrent);
  });

  connect(b2, &QPushButton::clicked, [this]()
  {
    requestNewEventState<State2>(NewEventStateMode::ExitCurrent);
  });

  connect(b3, &QPushButton::clicked, [this]()
  {
    requestExitEventState(ExitEventStateMode::ExitCurrent);
  });

  const auto layout = new QVBoxLayout();
  layout->addWidget(b1);
  layout->addWidget(b2);
  layout->addWidget(b3);
  layout->addStretch(1);

  m_overlay = new QtOsgBridge::VirtualOverlay();
  m_overlay->setLayout(layout);
  m_overlay->setGeometry(rand() % 700, rand() % 400, 300, 300);
  m_overlay->setStyleSheet("background-color: rgba(220, 100, 100, 50%);");

  m_overlayCompositor = mainWindow->getViewWidget()->getOverlayCompositor();
  m_overlayCompositor->addVirtualOverlay(m_overlay);

  m_overlay->show();
}

void State1::onExit()
{
  OSGG_LOG_DEBUG("onExit() State1");

  m_overlayCompositor->removeVirtualOverlay(m_overlay);
}

State2::State2(osgHelper::ioc::Injector& injector)
  : QtOsgBridge::EventProcessingState(injector)
{
}

State2::~State2()
{
  OSGG_LOG_DEBUG("~destruct State2");
}

void State2::onInitialize(QtOsgBridge::MainWindow* mainWindow)
{
  OSGG_LOG_DEBUG("onInitialize() State2");

  const auto b1 = new QPushButton("State 1");
  const auto b2 = new QPushButton("State 1 Exit");
  const auto b3 = new QPushButton("Close");

  connect(b1, &QPushButton::clicked, [this]()
  {
    requestNewEventState<State1>(NewEventStateMode::ContinueCurrent);
  });

  connect(b2, &QPushButton::clicked, [this]()
  {
    requestNewEventState<State1>(NewEventStateMode::ExitCurrent);
  });

  connect(b3, &QPushButton::clicked, [this]()
  {
    requestExitEventState(ExitEventStateMode::ExitCurrent);
  });

  const auto layout = new QVBoxLayout();
  layout->addWidget(b1);
  layout->addWidget(b2);
  layout->addWidget(b3);
  layout->addStretch(1);

  m_overlay = new QtOsgBridge::VirtualOverlay();
  m_overlay->setLayout(layout);
  m_overlay->setGeometry(rand() % 700, rand() % 400, 300, 300);
  m_overlay->setStyleSheet("background-color: rgba(100, 220, 100, 50%);");

  m_overlayCompositor = mainWindow->getViewWidget()->getOverlayCompositor();
  m_overlayCompositor->addVirtualOverlay(m_overlay);

  m_overlay->show();
}

void State2::onExit()
{
  OSGG_LOG_DEBUG("onExit() State2");

  m_overlayCompositor->removeVirtualOverlay(m_overlay);
}

InitialState::InitialState(osgHelper::ioc::Injector& injector)
  : QtOsgBridge::EventProcessingState(injector)
  , m_fxaa(injector.inject<osgHelper::ppu::FXAA>())
  , m_dof(injector.inject<osgHelper::ppu::DOF>())
  , m_hdr(injector.inject<osgHelper::ppu::HDR>())
  , m_rotation(0.0f)
  , m_position(0.0)
{
}

InitialState::~InitialState()
{
  OSGG_LOG_DEBUG("~destruct InitialState");
}

void InitialState::onInitialize(QtOsgBridge::MainWindow* mainWindow)
{
  OSGG_LOG_DEBUG("onInitialize() InitialState");

  auto geodeBox = new osg::Geode();
  geodeBox->addDrawable(new osg::ShapeDrawable(new osg::Box()));

  auto geodeSphere = new osg::Geode();
  geodeSphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));

  auto transformSphere = new osg::PositionAttitudeTransform();
  transformSphere->addChild(geodeSphere);
  transformSphere->setPosition(osg::Vec3d(2.0, 0.0, 0.0));

  auto transformBox = new osg::PositionAttitudeTransform();
  transformBox->addChild(geodeBox);
  
  auto updateTransformation = [this, transformBox]()
  {
    transformBox->setAttitude(osgHelper::getQuatFromEuler(0.3, 0.0, m_rotation));
    transformBox->setPosition(osg::Vec3d(0.0, 3.0 - 5.0 * cos(m_position), 0.0));
  };

  updateTransformation();

  auto group = new osg::Group();

  group->addChild(transformBox);
  group->addChild(transformSphere);

  group->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
  group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

  auto timer = new QTimer();
  timer->setInterval(16);
  timer->setSingleShot(false);
  QObject::connect(timer, &QTimer::timeout, [this, updateTransformation]()
  {
    m_rotation += 0.05f;
    m_position += 0.02;

    if (m_rotation >= 2.0f * C_PI)
    {
      m_rotation = 0.0f;
    }

    if (m_position >= 2.0 * C_PI)
    {
      m_position = 0.0;
    }

    updateTransformation();
  });

  timer->start();

  auto screenGeode = new osg::Geode();
  screenGeode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(100.0, 100.0, 0.0), osg::Vec3(200.0, 0.0, 0.0),
                                                           osg::Vec3(0.0, 100.0, 0.0)));


  auto sceneView = mainWindow->getViewWidget()->getView();
  auto sceneCamera = sceneView->getCamera(osgHelper::View::CameraType::Scene);

  sceneView->getRootGroup()->addChild(group);

  sceneCamera->setPosition(osg::Vec3f(0, -5, 0));
  sceneCamera->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));

  sceneView->setClampColorEnabled(true);

  const auto buttonsLayout = new QHBoxLayout();
  setupPostProcessingEffect<osgHelper::ppu::FXAA>(m_fxaa, sceneView, buttonsLayout, false);
  setupPostProcessingEffect<osgHelper::ppu::HDR>(m_hdr, sceneView, buttonsLayout, false);
  setupPostProcessingEffect<osgHelper::ppu::DOF>(m_dof, sceneView, buttonsLayout, false);

  const auto buttonState1 = new QPushButton("State 1");
  const auto buttonState2 = new QPushButton("State 2");
  const auto buttonExit = new QPushButton("Exit");

  connect(buttonState1, &QPushButton::clicked, [this]()
  {
    requestNewEventState<State1>(NewEventStateMode::ContinueCurrent);
  });

  connect(buttonState2, &QPushButton::clicked, [this]()
  {
    requestNewEventState<State2>(NewEventStateMode::ContinueCurrent);
  });

  connect(buttonExit, &QPushButton::clicked, [this]()
  {
    requestExitEventState(ExitEventStateMode::ExitAll);
  });

  const auto navLayout = new QHBoxLayout();
  navLayout->addWidget(buttonState1);
  navLayout->addWidget(buttonState2);
  navLayout->addWidget(buttonExit);

  const auto removeOverlayButton = new QPushButton("test");
  buttonsLayout->addWidget(removeOverlayButton);

  m_dof->setZNear(1.0f);
  m_dof->setZFar(1000.0f);
  m_dof->setFocalRange(3.0f);
  m_dof->setFocalLength(5.0f);

  auto overlayLayout = new QVBoxLayout();
  overlayLayout->addLayout(buttonsLayout);
  overlayLayout->addLayout(navLayout);
  overlayLayout->addStretch(1);

  auto overlay = new QtOsgBridge::VirtualOverlay();
  overlay->setLayout(overlayLayout);
  overlay->setGeometry(100, 100, 300, 300);
  overlay->setStyleSheet("background-color: rgba(100, 100, 220, 50%);");

  mainWindow->getViewWidget()->getOverlayCompositor()->addVirtualOverlay(overlay);

  m_mainWindow = mainWindow;
  m_overlay = overlay;

  connect(removeOverlayButton, &QPushButton::clicked, [this]()
  {
    m_mainWindow->getViewWidget()->getOverlayCompositor()->removeVirtualOverlay(m_overlay);
  });

  overlay->show();
}

void InitialState::onExit()
{
  OSGG_LOG_DEBUG("onExit() InitialState");
  m_mainWindow->getViewWidget()->getOverlayCompositor()->removeVirtualOverlay(m_overlay);
}

bool InitialState::onMouseEvent(QMouseEvent* event)
{
  if ((event->type() == QEvent::Type::MouseButtonDblClick) && (event->button() == Qt::MouseButton::LeftButton))
  {
    if (m_mainWindow->isFullScreen())
    {
      m_mainWindow->showNormal();
      return true;
    }
    m_mainWindow->showFullScreen();
    return true;
  }

  return false;
}

bool InitialState::onKeyEvent(QKeyEvent* event)
{
  if ((event->type() == QEvent::Type::KeyPress) && (event->key() == Qt::Key::Key_Escape))
  {
    m_mainWindow->close();
    return true;
  }

  return false;
}
