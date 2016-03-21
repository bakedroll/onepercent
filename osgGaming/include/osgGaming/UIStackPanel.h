#pragma once

#include <osgGaming/UIContainerElement.h>
#include <osgGaming/UICells.h>

namespace osgGaming
{
	class UIStackPanel : public UIContainerElement<int>
	{
	public:
		typedef enum _orientation
		{
			HORIZONTAL,
			VERTICAL
		} Orientation;

		UIStackPanel();

		osg::ref_ptr<UICells> getCells();

		virtual void getOriginSizeForLocationInArea(int location, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;
    Orientation getOrientation();

		void setSpacing(float spacing);
		void setOrientation(Orientation orientation);

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;

	private:
		float _spacing;
		Orientation _orientation;

		osg::ref_ptr<UICells> _cells;
	};
}