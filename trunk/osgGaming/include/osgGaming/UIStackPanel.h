#pragma once

#include <osgGaming/UIElement.h>

namespace osgGaming
{
	class UIStackPanel : public UIElement
	{
	public:
		typedef enum _orientation
		{
			HORIZONTAL,
			VERTICAL
		} Orientation;

		UIStackPanel();

		virtual void getOriginSizeForChildInArea(osg::ref_ptr<UIElement> child, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;

		void setSpacing(float spacing);
		void setOrientation(Orientation orientation);

	private:
		float _spacing;
		Orientation _orientation;

	};
}