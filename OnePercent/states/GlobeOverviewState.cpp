#include "GlobeOverviewState.h"

#include "nodes/CountryNameOverlay.h"

#include <osgViewer/ViewerEventHandlers>

#include <QtUtilsLib/Macros.h>

#include <osgHelper/View.h>

#include <osgHelper/ppu/FXAA.h>
#include <osgHelper/ppu/DOF.h>
#include <osgHelper/ppu/HDR.h>
#include <osgHelper/Helper.h>

#include <QKeyEvent>
#include <QDir>

namespace onep
{
  struct GlobeOverviewState::Impl
  {
    Impl(osgHelper::ioc::Injector& injector)
      : globeWorld(injector.inject<GlobeOverviewWorld>())
      , countryNameOverlay(injector.inject<CountryNameOverlay>())
      , timeSpeed(1.0f)
      , day(0.0f)
    {}

    osg::ref_ptr<GlobeOverviewWorld> globeWorld;
    osg::ref_ptr<CountryNameOverlay> countryNameOverlay;
    osg::ref_ptr<osgHelper::Camera>  camera;
    osg::ref_ptr<osgHelper::View>    view;

    QPointer<QtOsgBridge::MainWindow> mainWindow;

    float timeSpeed;
    float day;
  };

  const float NORMAL_TIME = 0.008f;

  GlobeOverviewState::GlobeOverviewState(osgHelper::ioc::Injector& injector)
    : EventProcessingState(injector)
    , m(new Impl(injector))
  {
  }

  GlobeOverviewState::~GlobeOverviewState() = default;

  void GlobeOverviewState::onInitialize(QPointer<QtOsgBridge::MainWindow> mainWindow, const SimulationData& data)
  {
    m->mainWindow = mainWindow;

    m->view   = mainWindow->getViewWidget()->getView();
    m->camera = m->view->getCamera(osgHelper::View::CameraType::Scene);
  }

  void GlobeOverviewState::onUpdate(const SimulationData& data)
  {
    // update visual time of year and day
    m->day += m->timeSpeed * NORMAL_TIME * data.timeDelta;

    m->globeWorld->setDay(m->camera, m->day);
  }

  bool GlobeOverviewState::onKeyEvent(QKeyEvent* event)
  {
    if (event->type() != QKeyEvent::Type::KeyPress)
    {
      return false;
    }

    switch (event->key())
    {
    case Qt::Key_Escape:
    {
      requestExitEventState(ExitGameStateMode::ExitAll);
      return true;
    }
    case Qt::Key_Q:
    {
      const auto enabled = !m->view->getPostProcessingEffectEnabled(osgHelper::ppu::HDR::Name);
      m->view->setPostProcessingEffectEnabled(osgHelper::ppu::HDR::Name, enabled);
      return true;
    }
    case Qt::Key_W:
    {
      const auto enabled = !m->view->getPostProcessingEffectEnabled(osgHelper::ppu::DOF::Name);
      m->view->setPostProcessingEffectEnabled(osgHelper::ppu::DOF::Name, enabled);
      return true;
    }
    case Qt::Key_E:
    {
      const auto enabled = !m->view->getPostProcessingEffectEnabled(osgHelper::ppu::FXAA::Name);

      if (enabled)
      {
        m->view->getPostProcessingEffect(osgHelper::ppu::FXAA::Name)->onResizeViewport(m->view->getResolution());
      }

      m->view->setPostProcessingEffectEnabled(osgHelper::ppu::FXAA::Name, enabled);
      return true;
    }
    case Qt::Key_F:
    {
      if (m->mainWindow->isFullScreen())
      {
        m->mainWindow->showNormal();
      }
      else
      {
        m->mainWindow->showFullScreen();
      }

      return true;
    }
    case Qt::Key_Minus:
    {
      m->timeSpeed *= 0.75f;
      UTILS_QLOG_DEBUG(QString("Speed: x%1").arg(m->timeSpeed));
      return true;
    }
    case Qt::Key_Plus:
    {
      m->timeSpeed *= 1.25f;
      UTILS_QLOG_DEBUG(QString("Speed: x%1").arg(m->timeSpeed));
      return true;
    }
    case Qt::Key_O:
    {
      osg::ref_ptr<CountryNameOverlay> overlay = m->countryNameOverlay;

      overlay->setEnabled(!overlay->getEnabled());

      return true;
    }
    case Qt::Key_F4:
    {
      UTILS_LOG_INFO("Capturing screenshot");

      osg::ref_ptr<osg::Image> capturedImage;
      osg::ref_ptr<osgViewer::ScreenCaptureHandler> screenCaptureHandler = new osgViewer::ScreenCaptureHandler();

      QDir dir("./screenshots");
      if (!dir.exists())
      {
        dir.mkdir(".");
      }

      screenCaptureHandler->setCaptureOperation(new osgViewer::ScreenCaptureHandler::WriteToFile(
        "./screenshots/capture",
        "png",
        osgViewer::ScreenCaptureHandler::WriteToFile::SEQUENTIAL_NUMBER));

      screenCaptureHandler->captureNextFrame(*m->mainWindow->getViewWidget()->getViewer());
      return true;
    }
    default:
      break;
    }

    return false;

    /*else
    {

    if (getViewer()->hasPostProcessingEffect(HighDynamicRangeEffect::NAME))
    {
    ref_ptr<HighDynamicRangeEffect> hdrEffect = dynamic_cast<HighDynamicRangeEffect*>(getViewer()->getPostProcessingEffect(HighDynamicRangeEffect::NAME).get());

    if (key == GUIEventAdapter::KEY_A)
    {
    hdrEffect->setMidGrey(hdrEffect->getMidGrey() + 0.5f);
    }
    else if (key == GUIEventAdapter::KEY_Y)
    {
    hdrEffect->setMidGrey(hdrEffect->getMidGrey() - 0.5f);
    }

    else if (key == GUIEventAdapter::KEY_S)
    {
    hdrEffect->setBlurSigma(hdrEffect->getBlurSigma() + 0.5f);
    }
    else if (key == GUIEventAdapter::KEY_X)
    {
    hdrEffect->setBlurSigma(hdrEffect->getBlurSigma() - 0.5f);
    }

    else if (key == GUIEventAdapter::KEY_D)
    {
    hdrEffect->setBlurRadius(hdrEffect->getBlurRadius() + 0.5f);
    }
    else if (key == GUIEventAdapter::KEY_C)
    {
    hdrEffect->setBlurRadius(hdrEffect->getBlurRadius() - 0.5f);
    }

    else if (key == GUIEventAdapter::KEY_F)
    {
    hdrEffect->setGlareFactor(hdrEffect->getGlareFactor() + 1.0f);
    }
    else if (key == GUIEventAdapter::KEY_V)
    {
    hdrEffect->setGlareFactor(hdrEffect->getGlareFactor() - 1.0f);
    }

    else if (key == GUIEventAdapter::KEY_F)
    {
    hdrEffect->setMinLuminance(hdrEffect->getMinLuminance() + 0.1f);
    }
    else if (key == GUIEventAdapter::KEY_V)
    {
    hdrEffect->setMinLuminance(hdrEffect->getMinLuminance() - 0.1f);
    }

    else if (key == GUIEventAdapter::KEY_G)
    {
    hdrEffect->setMaxLuminance(hdrEffect->getMaxLuminance() + 0.1f);
    }
    else if (key == GUIEventAdapter::KEY_B)
    {
    hdrEffect->setMaxLuminance(hdrEffect->getMaxLuminance() - 0.1f);
    }

    else if (key == GUIEventAdapter::KEY_H)
    {
    hdrEffect->setAdaptFactor(hdrEffect->getAdaptFactor() + 0.005f);
    }
    else if (key == GUIEventAdapter::KEY_N)
    {
    hdrEffect->setAdaptFactor(hdrEffect->getAdaptFactor() - 0.005f);
    }
    }

    }
    */
  }

}
