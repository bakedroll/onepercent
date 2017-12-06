#include "GlobeOverviewState.h"

#include "nodes/GlobeModel.h"

#include <osgViewer/ViewerEventHandlers>

#include <osgGaming/NativeView.h>

#include <osgGaming/HighDynamicRangeEffect.h>
#include <osgGaming/DepthOfFieldEffect.h>
#include <osgGaming/FastApproximateAntiAliasingEffect.h>

using namespace onep;
using namespace osg;
using namespace std;
using namespace osgGA;
using namespace osgGaming;

const float GlobeOverviewState::_NORMAL_TIME = 0.008f;

GlobeOverviewState::GlobeOverviewState()
	: QtGameState(),
	  _timeSpeed(1.0f),
	  _day(0.0f)
{
}

void GlobeOverviewState::initialize()
{
  _globeWorld = static_cast<GlobeOverviewWorld*>(getWorld(getView(0)).get());
}

unsigned char GlobeOverviewState::getProperties()
{
	return GameState::PROP_UPDATE_ALWAYS | PROP_UIMEVENTS_ALWAYS | PROP_GUIEVENTS_ALWAYS;
}

GameState::StateEvent* GlobeOverviewState::update()
{
	// update visual time of year and day
	_day += getFrameTime() * (_NORMAL_TIME * _timeSpeed);

	_globeWorld->setDay(_day);

	return stateEvent_default();
}

void GlobeOverviewState::onKeyPressedEvent(int key)
{
	if (key == GUIEventAdapter::KEY_Escape)
	{
		stateEvent_endGame();
	}
	else if (key == GUIEventAdapter::KEY_Q)
	{
		if (getView(0)->hasPostProcessingEffect(HighDynamicRangeEffect::NAME))
		{
      bool enabled = !getView(0)->getPostProcessingEffectEnabled(HighDynamicRangeEffect::NAME);

      getView(0)->setPostProcessingEffectEnabled(HighDynamicRangeEffect::NAME, enabled);
		}
	}
	else if (key == GUIEventAdapter::KEY_W)
	{
    if (getView(0)->hasPostProcessingEffect(DepthOfFieldEffect::NAME))
		{
      bool enabled = !getView(0)->getPostProcessingEffectEnabled(DepthOfFieldEffect::NAME);

      getView(0)->setPostProcessingEffectEnabled(DepthOfFieldEffect::NAME, enabled);
		}
	}
	else if (key == GUIEventAdapter::KEY_E)
	{
    if (getView(0)->hasPostProcessingEffect(FastApproximateAntiAliasingEffect::NAME))
		{
      bool enabled = !getView(0)->getPostProcessingEffectEnabled(FastApproximateAntiAliasingEffect::NAME);

			if (enabled)
			{
        static_cast<FastApproximateAntiAliasingEffect*>(getView(0)->getPostProcessingEffect(FastApproximateAntiAliasingEffect::NAME).get())->setResolution(getView(0)->getResolution());
			}

      getView(0)->setPostProcessingEffectEnabled(FastApproximateAntiAliasingEffect::NAME, enabled);
		}
	}
	else if (key == GUIEventAdapter::KEY_F)
	{
    bool bFullscreenEnabled = getFullscreenEnabledObs()->get();
    getFullscreenEnabledObs()->set(!bFullscreenEnabled);
	}
	else if (key == GUIEventAdapter::KEY_Minus)
	{
		_timeSpeed *= 0.75f;
		printf("Speed: x%f\n", _timeSpeed);
	}
	else if (key == GUIEventAdapter::KEY_Plus)
	{
		_timeSpeed *= 1.25f;
		printf("Speed: x%f\n", _timeSpeed);
	}
	else if (key == GUIEventAdapter::KEY_O)
	{
		ref_ptr<CountryOverlay> overlay = _globeWorld->getCountryOverlay();

		overlay->setEnabled(!overlay->getEnabled());
	}
  else if (key == GUIEventAdapter::KEY_F4)
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
