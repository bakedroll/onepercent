#pragma once

#include <osgGaming/UIElement.h>

namespace osgGaming
{
	class UIGrid : public UIElement
	{
	public:
		UIGrid();
		~UIGrid();

		virtual void getOriginSizeForChildInArea(unsigned int i, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;

		void setNumColumns(int columns);
		void setNumRows(int rows);

		void setSpacing(float spacing);

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;
		virtual void resetChildrenMinContentSize() override;

	private:
		osg::Vec2f minContentSizeAt(int col, int row);
		void calculateContentSizeArray();

		int _numColumns;
		int _numRows;

		float _spacing;

		//osg::Vec2f* _contentSizeArray;
		float* _minColumnSizeArray;
		float* _minRowSizeArray;

		bool _calculatedContentSizeArray;
	};
}