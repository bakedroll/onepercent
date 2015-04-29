#include <osgGaming/UIGrid.h>

#include <algorithm>

using namespace osgGaming;
using namespace osg;
using namespace std;

ColRow::_colRow()
	: col(0),
	  row(0)
{

}

ColRow::_colRow(int c, int r)
	: col(c),
	  row(r)
{

}

UIGrid::UIGrid()
	: UIContainerElement(),
	  _spacing(5.0f)
{
	_columns = new UICells();
	_rows = new UICells();

	_columns->setNumCells(1);
	_rows->setNumCells(1);
}

void UIGrid::getOriginSizeForLocationInArea(ColRow location, Vec2f area, Vec2f& origin, Vec2f& size)
{
	_columns->getActualOriginSize(location.col, area.x(), _spacing, origin.x(), size.x());
	_rows->getActualOriginSize(location.row, area.y(), _spacing, origin.y(), size.y());
}

void UIGrid::setSpacing(float spacing)
{
	_spacing = spacing;
}

ref_ptr<UICells> UIGrid::getRows()
{
	return _rows;
}

ref_ptr<UICells> UIGrid::getColumns()
{
	return _columns;
}

Vec2f UIGrid::calculateMinContentSize()
{
	_columns->reset();
	_rows->reset();

	UIElementList children = getUIChildren();

	for (UIElementList::iterator it = children.begin(); it != children.end(); ++it)
	{
		ref_ptr<UIElement> child = *it;

		ColRow rowCol = getLocationOfChild(child);
		Vec2f minSize = child->getMinSize();

		_columns->expandCell(rowCol.col, minSize.x());
		_rows->expandCell(rowCol.row, minSize.y());
	}


	float minContentWidth = ((float)(_columns->getNumCells() - 1) * _spacing);
	float minContentHeight = ((float)(_rows->getNumCells() - 1) * _spacing);

	for (int c = 0; c < _columns->getNumCells(); c++)
	{
		float size;
		_columns->getMinSize(c, size);

		minContentWidth += size;
	}

	for (int r = 0; r < _rows->getNumCells(); r++)
	{
		float size;
		_rows->getMinSize(r, size);

		minContentHeight += size;
	}

	return Vec2f(minContentWidth, minContentHeight);
}
