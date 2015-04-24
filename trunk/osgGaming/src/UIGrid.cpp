#include <osgGaming/UIGrid.h>

#include <algorithm>

using namespace osgGaming;
using namespace osg;
using namespace std;

UIGrid::UIGrid()
	: UIElement(),
	  _numColumns(1),
	  _numRows(1),
	  _spacing(5.0f),
	  _calculatedContentSizeArray(false)
{

}

UIGrid::~UIGrid()
{
	if (_calculatedContentSizeArray == true)
	{
		delete[] _minColumnSizeArray;
		delete[] _minRowSizeArray;
	}
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

Vec2f UIGrid::calculateMinContentSize()
{
	float minContentWidth = ((float)(_numColumns - 1) * _spacing);
	float minContentHeight = ((float)(_numRows - 1) * _spacing);

	for (int c = 0; c < _numColumns; c++)
	{
		minContentWidth += minContentSizeAt(c, 0).x();
	}

	for (int r = 0; r < _numRows; r++)
	{
		minContentHeight += minContentSizeAt(0, r).y();
	}

	return Vec2f(minContentWidth, minContentHeight);
}

void UIGrid::resetChildrenMinContentSize()
{
	UIElementList children = getUIChildren();

	for (UIElementList::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->resetMinContentSize();
	}

	if (_calculatedContentSizeArray == true)
	{
		delete[] _minColumnSizeArray;
		delete[] _minRowSizeArray;
		_calculatedContentSizeArray = false;
	}
}

Vec2f UIGrid::minContentSizeAt(int col, int row)
{
	if (_calculatedContentSizeArray == false)
	{
		calculateContentSizeArray();
		_calculatedContentSizeArray = true;
	}

	return Vec2f(_minColumnSizeArray[col], _minRowSizeArray[row]);
}

void UIGrid::calculateContentSizeArray()
{
	UIElementList children = getUIChildren();

	int numChildren = min((int)children.size(), _numColumns * _numRows);

	Vec2f* contentSizeArray = new Vec2f[numChildren];

	for (int i = 0; i < numChildren; i++)
	{
		Vec4f margin = children[i]->getMargin();
		Vec4f padding = children[i]->getPadding();

		contentSizeArray[i] = children[i]->getMinSize();
	}

	_minColumnSizeArray = new float[_numColumns];
	_minRowSizeArray = new float[_numRows];

	for (int c = 0; c < _numColumns; c++)
	{
		_minColumnSizeArray[c] = 0.0f;

		for (int r = 0; r < _numRows; r++)
		{
			int array_i = c * _numColumns + r;
			if (array_i < numChildren)
				_minColumnSizeArray[c] = fmaxf(_minColumnSizeArray[c], contentSizeArray[array_i].x());
		}
	}

	for (int r = 0; r < _numRows; r++)
	{
		_minRowSizeArray[r] = 0.0f;

		for (int c = 0; c < _numColumns; c++)
		{
			int array_i = c * _numColumns + r;
			if (array_i < numChildren)
				_minRowSizeArray[r] = fmaxf(_minRowSizeArray[r], contentSizeArray[array_i].y());
		}
	}

	delete[] contentSizeArray;
}
