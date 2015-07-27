#pragma once

#include "GlobeOverviewWorld.h"

#include <osgGaming/GameState.h>
#include <osgGaming/Animation.h>
#include <osgGaming/UserInteractionModel.h>

namespace onep
{
	class GlobeOverviewState : public osgGaming::GameState
	{
	public:
		GlobeOverviewState();

		virtual void initialize() override;

		virtual unsigned char getProperties() override;

		virtual StateEvent* update() override;

		virtual void onKeyPressedEvent(int key) override;

		//virtual void onUIClickedEvent(osg::ref_ptr<UIElement> uiElement) override;

	private:
		static const float _NORMAL_TIME;

		osg::ref_ptr<GlobeOverviewWorld> _globeWorld;

		float _timeSpeed;
		float _day;
	};
}