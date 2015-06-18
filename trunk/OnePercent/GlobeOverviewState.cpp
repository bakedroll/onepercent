#include "GlobeOverviewState.h"
#include "GlobeModel.h"
#include "BackgroundModel.h"

#include <osgGaming/Helper.h>
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
	: GameState(),
	  _timeSpeed(1.0f),
	  _day(0.0f)
{
}

void GlobeOverviewState::initialize()
{
	_globeWorld = static_cast<GlobeOverviewWorld*>(getWorld().get());

	_globeWorld->getBackgroundModel()->updateResolutionHeight(getViewer()->getResolution().y());
}

unsigned char GlobeOverviewState::getProperties()
{
	return GameState::PROP_ENABLED | GameState::PROP_RUN_ALWAYS;
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
	else if (key == GUIEventAdapter::KEY_1)
	{
		if (getViewer()->hasPostProcessingEffect(HighDynamicRangeEffect::NAME))
		{
			bool enabled = !getViewer()->getPostProcessingEffectEnabled(HighDynamicRangeEffect::NAME);

			getViewer()->setPostProcessingEffectEnabled(HighDynamicRangeEffect::NAME, enabled);
		}
	}
	else if (key == GUIEventAdapter::KEY_2)
	{
		if (getViewer()->hasPostProcessingEffect(DepthOfFieldEffect::NAME))
		{
			bool enabled = !getViewer()->getPostProcessingEffectEnabled(DepthOfFieldEffect::NAME);

			getViewer()->setPostProcessingEffectEnabled(DepthOfFieldEffect::NAME, enabled);
		}
	}
	else if (key == GUIEventAdapter::KEY_3)
	{
		if (getViewer()->hasPostProcessingEffect(FastApproximateAntiAliasingEffect::NAME))
		{
			bool enabled = !getViewer()->getPostProcessingEffectEnabled(FastApproximateAntiAliasingEffect::NAME);

			if (enabled)
			{
				static_cast<FastApproximateAntiAliasingEffect*>(getViewer()->getPostProcessingEffect(FastApproximateAntiAliasingEffect::NAME).get())->setResolution(getViewer()->getResolution());
			}

			getViewer()->setPostProcessingEffectEnabled(FastApproximateAntiAliasingEffect::NAME, enabled);
		}
	}
	else if (key == GUIEventAdapter::KEY_0)
	{
		//getViewer()->setWindowedResolution(Vec2f(800, 600));

		getViewer()->setFullscreenEnabled(!getViewer()->getFullscreenEnabled());
	}
	else if (key == GUIEventAdapter::KEY_Q)
	{
		_timeSpeed *= 0.75f;
		printf("Speed: x%f\n", _timeSpeed);
	}
	else if (key == GUIEventAdapter::KEY_W)
	{
		_timeSpeed *= 1.25f;
		printf("Speed: x%f\n", _timeSpeed);
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

void GlobeOverviewState::onUIMClickedEvent(UserInteractionModel* model)
{
	printf("Clicked UIM: %s\n", model->getUIMName().data());
}