#pragma once

#include "GlobeCameraState.h"

#include "core/Globals.h"
#include "nodes/GlobeOverviewWorld.h"

#include <osgGaming/UIText.h>
#include <osgGaming/Property.h>

#include <osgGaming/Timer.h>
#include <osgGaming/UIRadioGroup.h>

namespace onep
{
	class GlobeInteractionState : public GlobeCameraState
	{
	public:
		GlobeInteractionState();

		virtual void initialize() override;
    void setupUi();

		virtual void onMousePressedEvent(int button, float x, float y) override;
		virtual void onKeyPressedEvent(int key) override;

		virtual void onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion) override;

		virtual void onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position, osg::Vec2f change) override;
		virtual void onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position) override;

		virtual void onUIClickedEvent(osg::ref_ptr<osgGaming::UIElement> uiElement) override;

		void dayTimerElapsed();

		void startSimulation();

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> overrideHud(osg::ref_ptr<osgGaming::View> view) override;

	private:
		float m_paramEarthRadius;
		float m_paramCameraMinDistance;
		float m_paramCameraMaxDistance;
		float m_paramCameraMaxLatitude;
		float m_paramCameraZoomSpeed;
		float m_paramCameraZoomSpeedFactor;
		float m_paramCameraScrollSpeed;
		float m_paramCameraRotationSpeed;

		bool ready();

		void updateCountryInfoText();

		osg::ref_ptr<osgGaming::UIText> _textPleaseSelect;
		osg::ref_ptr<osgGaming::UIText> _textConfirm;
		osg::ref_ptr<osgGaming::UIText> _textProgress;
		osg::ref_ptr<osgGaming::UIText> _textCountryInfo;

		bool _ready;
		bool _started;

		int _selectedCountry;

		osg::ref_ptr<osgGaming::Timer> _simulationTimer;

    osgGaming::UIRadioGroup::Ptr m_branchRadioGroup;

    osgGaming::Observer<int>::Ptr m_selectedCountryObserver;
    osgGaming::Observer<bool>::Ptr m_skillBranchActivatedObserver;
    osgGaming::Observer<osgGaming::UIButton::Ptr>::Ptr m_selectedButtonObserver;

    osgGaming::Observer<int>::Ptr m_selectedCountryIdObserver;
	};
}
