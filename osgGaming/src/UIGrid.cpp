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
	  _calculatedMinSizeArray(false),
	  _calculatedActualSizeArray(false)
{

}

UIGrid::~UIGrid()
{
	if (_calculatedMinSizeArray == true)
	{
		delete[] _rows;
		delete[] _columns;
	}
}

bool UIGrid::addChild(Node* node)
{
	return addChild(node, 0, 0);
}

bool UIGrid::addChild(Node* node, int col, int row)
{
	ref_ptr<UIElement> uiElement = dynamic_cast<UIElement*>(node);
	if (uiElement.valid())
	{
		ChildRowColMap::iterator it = _childRowColMap.find(uiElement);

		if (it != _childRowColMap.end())
		{
			// TODO: Exception
			printf("Inconsistent state\n");
		}

		RowCol rowCol;
		rowCol.col = col;
		rowCol.row = row;

		_childRowColMap.insert(ChildRowColMap::value_type(uiElement, rowCol));
	}

	return MatrixTransform::addChild(node);
}

void UIGrid::getOriginSizeForChildInArea(osg::ref_ptr<UIElement> child, Vec2f area, Vec2f& origin, Vec2f& size)
{

	//int row = i / _numColumns;
	//int col = i % _numColumns;

	RowCol rowCol = _childRowColMap.find(child)->second;

	actualOriginSizeAt(rowCol.col, rowCol.row, area, origin, size);

	/*if (rowCol.row >= _numRows)
	{
		// TODO: Exception
		rowCol.row = _numRows - 1;
		rowCol.col = _numColumns - 1;
	}

	size.x() = (area.x() - ((float)(_numColumns - 1) * _spacing)) / (float)_numColumns;
	size.y() = (area.y() - ((float)(_numRows - 1) * _spacing)) / (float)_numRows;

	origin.x() = (float)rowCol.col * (size.x() + _spacing);
	origin.y() = (float)rowCol.row * (size.y() + _spacing);*/
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
		minContentWidth += minSizeAt(c, 0).x();
	}

	for (int r = 0; r < _numRows; r++)
	{
		minContentHeight += minSizeAt(0, r).y();
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

	if (_calculatedMinSizeArray == true)
	{
		delete[] _rows;
		delete[] _columns;
		_calculatedMinSizeArray = false;
	}

	_calculatedActualSizeArray = false;
}

void UIGrid::cellsAt(int col, int row, Cell& colCell, Cell& rowCell)
{
	if (_calculatedMinSizeArray == false)
	{
		calculateMinSizeArray();
		_calculatedMinSizeArray = true;
	}

	colCell = _columns[col];
	rowCell = _rows[row];

	/*for (int c = 0; c < _numRows; c++)
	{
		if (_columns[c].position == col)
		{
			colCell = _columns[c];
			break;
		}
	}

	for (int r = 0; r < _numRows; r++)
	{
		if (_rows[r].position == row)
		{
			rowCell = _rows[r];
			break;
		}
	}*/
}

Vec2f UIGrid::minSizeAt(int col, int row)
{
	Cell colCell, rowCell;
	cellsAt(col, row, colCell, rowCell);

	return Vec2f(colCell.minSize, rowCell.minSize);
}

void UIGrid::actualOriginSizeAt(int col, int row, Vec2f area, Vec2f& origin, Vec2f& size)
{
	if (_calculatedActualSizeArray == false)
	{
		calculateActualSizeArray(area);
		_calculatedActualSizeArray = true;
	}

	Cell colCell, rowCell;
	cellsAt(col, row, colCell, rowCell);

	origin = Vec2f(colCell.actualOrigin, rowCell.actualOrigin);
	size = Vec2f(colCell.actualSize, rowCell.actualSize);
}

void UIGrid::calculateMinSizeArray()
{
	_rows = new Cell[_numRows];
	_columns = new Cell[_numColumns];

	for (int r = 0; r < _numRows; r++)
	{
		_rows[r].minSize = 0.0f;
		_rows[r].index = r;
	}

	for (int c = 0; c < _numColumns; c++)
	{
		_columns[c].minSize = 0.0f;
		_columns[c].index = c;
	}

	UIElementList children = getUIChildren();

	for (UIElementList::iterator it = children.begin(); it != children.end(); ++it)
	{
		ref_ptr<UIElement> child = *it;

		RowCol rowCol = _childRowColMap.find(child)->second;
		Vec2f minSize = child->getMinSize();

		_columns[rowCol.col].minSize = fmaxf(_columns[rowCol.col].minSize, minSize.x());
		_rows[rowCol.row].minSize = fmaxf(_rows[rowCol.row].minSize, minSize.y());
	}
}

void UIGrid::calculateActualSizeArray(Vec2f area)
{
	vector<Cell> columns(_columns, _columns + _numColumns);
	vector<Cell> rows(_rows, _rows + _numRows);

	sort(columns.begin(), columns.end(), compareCells);
	sort(rows.begin(), rows.end(), compareCells);

	


	float remainingColSize = area.x();
	int currentColIndex = 0;
	for (vector<Cell>::iterator it = columns.begin(); it != columns.end(); ++it)
	{
		float avColSize = (remainingColSize - ((float)(_numColumns - currentColIndex - 1) * _spacing))
			/ (float)(_numColumns - currentColIndex);

		_columns[it->index].actualSize = fmaxf(_columns[it->index].minSize, avColSize);

		remainingColSize -= (_columns[it->index].actualSize + _spacing);
		currentColIndex++;
	}

	float colOrigin = 0.0f;
	for (int c = 0; c < _numColumns; c++)
	{
		_columns[c].actualOrigin = colOrigin;
		colOrigin += (_columns[c].actualSize + _spacing);
	}


	float remainingRowSize = area.y();
	int currentRowIndex = 0;
	for (vector<Cell>::iterator it = rows.begin(); it != rows.end(); ++it)
	{
		float avRowSize = (remainingRowSize - ((float)(_numRows - currentRowIndex - 1) * _spacing))
			/ (float)(_numRows - currentRowIndex);

		_rows[it->index].actualSize = fmaxf(_rows[it->index].minSize, avRowSize);

		remainingRowSize -= (_rows[it->index].actualSize + _spacing);
		currentRowIndex++;
	}

	float rowOrigin = 0.0f;
	for (int r = 0; r < _numRows; r++)
	{
		_rows[r].actualOrigin = rowOrigin;
		rowOrigin += (_rows[r].actualSize + _spacing);
	}
}

bool UIGrid::compareCells(Cell i, Cell j)
{
	return i.minSize > j.minSize;
}