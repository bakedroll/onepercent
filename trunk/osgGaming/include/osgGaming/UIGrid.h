#pragma once

#include <map>

#include <osgGaming/UIContainerElement.h>
#include <osgGaming/UICells.h>

namespace osgGaming
{
	typedef struct _colRow
	{
		_colRow();
		_colRow(int c, int r);

		int col;
		int row;
	} ColRow;

	class UIGrid : public UIContainerElement<ColRow>
	{
	public:
		UIGrid();

		virtual void getOriginSizeForLocationInArea(ColRow location, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;

		void setNumColumns(int columns);
		void setNumRows(int rows);

		void setSpacing(float spacing);

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;

	private:
		int _numColumns;
		int _numRows;

		float _spacing;

		osg::ref_ptr<UICells> _rows;
		osg::ref_ptr<UICells> _columns;
	};
}