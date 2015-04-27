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

		virtual void getOriginSizeForLocationInArea(int location, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;

		void setSpacing(float spacing);
		void setOrientation(Orientation orientation);

		void setNumCells(int num);

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;

	private:
		float _spacing;
		Orientation _orientation;

		int _numCells;

		osg::ref_ptr<UICells> _cells;
	};
}