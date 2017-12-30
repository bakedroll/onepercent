#pragma once

#include "GlobeCameraState.h"

#include <osgGA/GUIEventAdapter>

#include <memory>

namespace onep
{
  class VirtualOverlay;

	class GlobeInteractionState : public GlobeCameraState
	{
	public:
		GlobeInteractionState();
		~GlobeInteractionState();

		virtual void initialize() override;

    virtual StateEvent* update() override;

		virtual void onMousePressedEvent(int button, float x, float y) override;
		virtual void onKeyPressedEvent(int key) override;
    virtual void onMouseMoveEvent(float x, float y) override;

		virtual void onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion) override;

		virtual void onDragEvent(int button, osg::Vec2f origin, osg::Vec2f position, osg::Vec2f change) override;
		virtual void onDragEndEvent(int button, osg::Vec2f origin, osg::Vec2f position) override;

    virtual void onResizeEvent(float width, float height) override;

		void dayTimerElapsed();

		void startSimulation();

	protected:
		virtual osg::ref_ptr<osgGaming::Hud> overrideHud(osg::ref_ptr<osgGaming::View> view) override;
    void setupUi();

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
