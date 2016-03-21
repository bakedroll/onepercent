#pragma once

#include <osg/Referenced>

namespace osgGaming
{
	class UICells : public osg::Referenced
	{
	public:
		typedef enum _sizePolicy
		{
			AUTO,
			CONTENT
		} SizePolicy;

		UICells();
		~UICells();

		void expandCell(int index, float size);

    void setDefaultSizePolicy(SizePolicy sizePolicy);
		void setNumCells(int num);
		void setSizePolicy(int cell, SizePolicy sizePolicy);

		int getNumCells();
		SizePolicy getSizePolicy(int cell);

		void getMinSize(int index, float& minSize);
		void getActualOriginSize(int index, float totalSize, float spacing, float& origin, float& size/*, bool reverseIdx*/);

		void reset();

	private:
		typedef struct _cell
		{
			int index;
			float minSize;
			float actualSize;
			float actualOrigin;
			SizePolicy sizePolicy;
		} Cell;

		void calculateActualSize(float totalSize, float spacing);

		static bool compareCells(Cell i, Cell j);

		Cell* _cells;
		int _numCells;

		bool _calculatedActualSize;
		bool _initialized;
    SizePolicy _defaultSizePolicy;
	};
}