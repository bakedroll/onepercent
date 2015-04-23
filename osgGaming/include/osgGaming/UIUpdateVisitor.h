#pragma once

#include <osg/NodeVisitor>
#include <osgGaming/UIElement.h>

namespace osgGaming
{
	class UIUpdateVisitor : public osg::NodeVisitor
	{
	public:
		UIUpdateVisitor();

		virtual void apply(osg::Node &node) override;

	private:
		void getOriginSizeInArea(osg::ref_ptr<UIElement> element, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size);

	};
}