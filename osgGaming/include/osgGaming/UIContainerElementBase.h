#pragma once

#include <osgGaming/UIElement.h>

#include <vector>

namespace osgGaming
{
	class UIContainerElementBase : public UIElement
	{
	public:
		typedef std::vector<osg::ref_ptr<UIElement>> UIElementList;

		UIContainerElementBase();

		UIElementList getUIChildren();
		unsigned int getNumUIChildren();

		virtual void getOriginSizeForChildInArea(osg::ref_ptr<UIElement> child, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) = 0;

	protected:
		virtual void onResetMinContentSize() override final;

	};
}
