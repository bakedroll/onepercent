#pragma once

#include <osgGaming/UIElement.h>

namespace osgGaming
{
	class UIVisualElement : public UIElement
	{
	public:
		UIVisualElement();

		virtual void updatedContentOriginSize(osg::Vec2f origin, osg::Vec2f size);
	};
}