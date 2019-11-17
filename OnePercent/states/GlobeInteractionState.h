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
		GlobeInteractionState(osgGaming::Injector& injector);
		~GlobeInteractionState();

		void initialize() override;

    StateEvent* update() override;

		void onMousePressedEvent(int button, float x, float y) override;
		void onKeyPressedEvent(int key) override;
    void onMouseMoveEvent(float x, float y) override;

		void onScrollEvent(osgGA::GUIEventAdapter::ScrollingMotion motion) override;

    void onDragBeginEvent(int button, const osg::Vec2f& position) override;
    void onDragEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position,
                     const osg::Vec2f& change) override;
    void onDragEndEvent(int button, const osg::Vec2f& origin, const osg::Vec2f& position) override;

    void onResizeEvent(float width, float height) override;

	protected:
    osg::ref_ptr<osgGaming::Hud> injectHud(osgGaming::Injector& injector, const osg::ref_ptr<osgGaming::View>& view) override;
    void setupUi();

	private:
		struct Impl;
		std::unique_ptr<Impl> m;

	};
}
