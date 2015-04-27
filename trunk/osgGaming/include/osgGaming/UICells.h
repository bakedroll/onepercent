#pragma once

#include <osg/Referenced>
#include <osg/Vec2>

namespace osgGaming
{
	class UICells : public osg::Referenced
	{
	public:
		UICells();
		~UICells();

		void expandCell(int index, float size);

		void setNumCells(int num);

		void getMinSize(int index, float& minSize);
		void getActualOriginSize(int index, float totalSize, float spacing, float& origin, float& size);

	private:
		typedef struct _cell
		{
			int index;
			float minSize;
			float actualSize;
			float actualOrigin;
		} Cell;

		void calculateActualSize(float totalSize, float spacing);

		static bool compareCells(Cell i, Cell j);

		Cell* _cells;
		int _numCells;

		bool _calculatedActualSize;
		bool _initialized;
	};
}