#include <osgGaming/UICells.h>

#include <vector>
#include <algorithm>

using namespace osgGaming;
using namespace osg;
using namespace std;

UICells::UICells()
	: Referenced(),
	  _calculatedActualSize(false),
	  _initialized(false)
{
}

UICells::~UICells()
{
	if (_initialized == true)
	{
		delete[] _cells;
	}
}

void UICells::expandCell(int index, float size)
{
	_cells[index].minSize = fmaxf(_cells[index].minSize, size);
}

void UICells::setNumCells(int num)
{
	if (_initialized == true)
	{
		delete[] _cells;
	}

	_numCells = num;

	_cells = new Cell[_numCells];

	_calculatedActualSize = false;

	for (int i = 0; i < _numCells; i++)
	{
		_cells[i].index = i;
		_cells[i].minSize = 0.0f;
		_cells[i].actualSize = 0.0f;
		_cells[i].actualOrigin = 0.0f;
	}
}

void UICells::getMinSize(int index, float& minSize)
{
	minSize = _cells[index].minSize;
}

void UICells::getActualOriginSize(int index, float totalSize, float spacing, float& origin, float& size)
{
	if (_calculatedActualSize == false)
	{
		calculateActualSize(totalSize, spacing);
		_calculatedActualSize = true;
	}

	origin = _cells[index].actualOrigin;
	size = _cells[index].actualSize;
}

void UICells::calculateActualSize(float totalSize, float spacing)
{
	vector<Cell> cells(_cells, _cells + _numCells);

	sort(cells.begin(), cells.end(), compareCells);

	float remainingSize = totalSize;
	int currentIndex = 0;
	for (vector<Cell>::iterator it = cells.begin(); it != cells.end(); ++it)
	{
		float avSize = (remainingSize - ((float)(_numCells - currentIndex - 1) * spacing))
			/ (float)(_numCells - currentIndex);

		_cells[it->index].actualSize = fmaxf(_cells[it->index].minSize, avSize);

		remainingSize -= (_cells[it->index].actualSize + spacing);
		currentIndex++;
	}

	float origin = 0.0f;
	for (int i = 0; i < _numCells; i++)
	{
		_cells[i].actualOrigin = origin;
		origin += (_cells[i].actualSize + spacing);
	}
}


bool UICells::compareCells(Cell i, Cell j)
{
	return i.minSize > j.minSize;
}