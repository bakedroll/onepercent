#include <osgGaming/UIGrid.h>

using namespace osgGaming;
using namespace osg;

UIGrid::UIGrid()
	: UIElement(),
	  _numColumns(1),
	  _numRows(1),
	  _spacing(5.0f)
{

}

void UIGrid::getOriginSizeForChildInArea(unsigned int i, Vec2f area, Vec2f& origin, Vec2f& size)
{
	int row = i / _numColumns;
	int col = i % _numColumns;

	if (row >= _numRows)
	{
		// TODO: Exception
		row = _numRows-1;
		col = _numColumns-1;
	}

	size.x() = (area.x() - ((float)(_numColumns - 1) * _spacing)) / (float)_numColumns;
	size.y() = (area.y() - ((float)(_numRows - 1) * _spacing)) / (float)_numRows;

	origin.x() = (float)col * (size.x() + _spacing);
	origin.y() = (float)row * (size.y() + _spacing);
}

void UIGrid::setNumColumns(int columns)
{
	_numColumns = columns;
}

void UIGrid::setNumRows(int rows)
{
	_numRows = rows;
}

void UIGrid::setSpacing(float spacing)
{
	_spacing = spacing;
}