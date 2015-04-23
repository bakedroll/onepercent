#pragma once

#include <osgGaming/UIElement.h>

namespace osgGaming
{
	class UIGrid : public UIElement
	{
	public:
		UIGrid();

		virtual void getOriginSizeForChildInArea(unsigned int i, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;

		void setNumColumns(int columns);
		void setNumRows(int rows);

		void setSpacing(float spacing);

	private:
		int _numColumns;
		int _numRows;

		float _spacing;
	};
}