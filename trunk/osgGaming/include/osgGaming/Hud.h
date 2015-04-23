#pragma once

#include <osgGaming/UIElement.h>

#include <osg/Referenced>
#include <osg/Projection>
#include <osg/Geode>

#include <osgText/Text>

namespace osgGaming
{
	class Hud : public osg::Referenced
	{
	public:
		Hud();

		osg::ref_ptr<osg::Projection> getProjection();

		// TODO: remove later
		osg::ref_ptr<osg::Geode> getGeode();
		osg::ref_ptr<UIElement> getRootUIElement();

		void updateResolution(unsigned int width, unsigned int height);
		void updateUIElements();

		void setFpsEnabled(bool enabled);

		void setRootUIElement(osg::ref_ptr<UIElement> element);

	private:

		osg::ref_ptr<osg::Projection> _projection;
		osg::ref_ptr<osg::Geode> _geode;

		osg::ref_ptr<UIElement> _rootUIElement;

		osg::ref_ptr<osgText::Text> _fpsText;
		bool _fpsEnabled;

		osg::Vec2f _resolution;
	};
}