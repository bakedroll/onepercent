#include "GlobeOverviewState.h"

#include "nodes/GlobeModel.h"

#include <osgViewer/ViewerEventHandlers>

#include <osgGaming/NativeView.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>

namespace onep
{
  struct GlobeOverviewState::Impl
  {
    Impl(osgGaming::Injector& injector)
      : globeWorld(injector.inject<GlobeOverviewWorld>())
      , countryNameOverlay(injector.inject<CountryNameOverlay>())
      , timeSpeed(1.0f)
      , day(0.0f)
    {}

    osg::ref_ptr<GlobeOverviewWorld> globeWorld;
    osg::ref_ptr<CountryNameOverlay> countryNameOverlay;

    float timeSpeed;
    float day;
  };

  const float NORMAL_TIME = 0.008f;

  GlobeOverviewState::GlobeOverviewState(osgGaming::Injector& injector)
    : QtGameState()
    , m(new Impl(injector))
  {
  }

  GlobeOverviewState::~GlobeOverviewState()
  {
  }

  unsigned char GlobeOverviewState::getProperties()
  {
    return GameState::PROP_UPDATE_ALWAYS | PROP_UIMEVENTS_ALWAYS | PROP_GUIEVENTS_ALWAYS;
  }

  osgGaming::GameState::StateEvent* GlobeOverviewState::update()
  {
    // update visual time of year and day
    m->day += getFrameTime() * (NORMAL_TIME * m->timeSpeed);

    m->globeWorld->setDay(m->day);

    return stateEvent_default();
  }

  void GlobeOverviewState::onKeyPressedEvent(int key)
  {
    if (key == osgGA::GUIEventAdapter::KEY_Escape)
    {
      stateEvent_endGame();
    }
    else if (key == osgGA::GUIEventAdapter::KEY_Q)
    {
      if (getView(0)->hasPostProcessingEffect(osgGaming::HighDynamicRangeEffect::NAME))
      {
        bool enabled = !getView(0)->getPostProcessingEffectEnabled(osgGaming::HighDynamicRangeEffect::NAME);

        getView(0)->setPostProcessingEffectEnabled(osgGaming::HighDynamicRangeEffect::NAME, enabled);
      }
    }
    else if (key == osgGA::GUIEventAdapter::KEY_W)
    {
      if (getView(0)->hasPostProcessingEffect(osgGaming::DepthOfFieldEffect::NAME))
      {
        bool enabled = !getView(0)->getPostProcessingEffectEnabled(osgGaming::DepthOfFieldEffect::NAME);

        getView(0)->setPostProcessingEffectEnabled(osgGaming::DepthOfFieldEffect::NAME, enabled);
      }
    }
    else if (key == osgGA::GUIEventAdapter::KEY_E)
    {
      if (getView(0)->hasPostProcessingEffect(osgGaming::FastApproximateAntiAliasingEffect::NAME))
      {
        bool enabled = !getView(0)->getPostProcessingEffectEnabled(osgGaming::FastApproximateAntiAliasingEffect::NAME);

        if (enabled)
        {
          static_cast<osgGaming::FastApproximateAntiAliasingEffect*>(getView(0)->getPostProcessingEffect(osgGaming::FastApproximateAntiAliasingEffect::NAME).get())->setResolution(getView(0)->getResolution());
        }

        getView(0)->setPostProcessingEffectEnabled(osgGaming::FastApproximateAntiAliasingEffect::NAME, enabled);
      }
    }
    else if (key == osgGA::GUIEventAdapter::KEY_F)
    {
      bool bFullscreenEnabled = getFullscreenEnabledObs()->get();
      getFullscreenEnabledObs()->set(!bFullscreenEnabled);
    }
    else if (key == osgGA::GUIEventAdapter::KEY_Minus)
    {
      m->timeSpeed *= 0.75f;
      printf("Speed: x%f\n", m->timeSpeed);
    }
    else if (key == osgGA::GUIEventAdapter::KEY_Plus)
    {
      m->timeSpeed *= 1.25f;
      printf("Speed: x%f\n", m->timeSpeed);
    }
    else if (key == osgGA::GUIEventAdapter::KEY_O)
    {
      osg::ref_ptr<CountryNameOverlay> overlay = m->countryNameOverlay;

      overlay->setEnabled(!overlay->getEnabled());
    }
    else if (key == osgGA::GUIEventAdapter::KEY_F4)
    {
      printf("Capturing screenshot\n");

      osg::ref_ptr<osg::Image> capturedImage;
      osg::ref_ptr<osgViewer::ScreenCaptureHandler> screenCaptureHandler = new osgViewer::ScreenCaptureHandler();

      screenCaptureHandler->setCaptureOperation(new osgViewer::ScreenCaptureHandler::WriteToFile(
        "./screenshots/capture",
        "png",
        osgViewer::ScreenCaptureHandler::WriteToFile::SEQUENTIAL_NUMBER));

      screenCaptureHandler->captureNextFrame(*getViewer());
    }
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