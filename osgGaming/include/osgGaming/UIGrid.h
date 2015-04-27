#pragma once

#include <map>

#include <osgGaming/UIElement.h>

namespace osgGaming
{
	class UIGrid : public UIElement
	{
	public:
		typedef struct _rowCol
		{
			int col;
			int row;
		} RowCol;

		typedef struct _cell
		{
			int index;
			float minSize;
			float actualSize;
			float actualOrigin;
		} Cell;

		typedef std::map<osg::ref_ptr<UIElement>, RowCol> ChildRowColMap;

		UIGrid();
		~UIGrid();

		virtual bool addChild(osg::Node* node) override;
		virtual bool addChild(osg::Node* node, int col, int row);

		virtual void getOriginSizeForChildInArea(osg::ref_ptr<UIElement> child, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;

		void setNumColumns(int columns);
		void setNumRows(int rows);

		void setSpacing(float spacing);

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;
		virtual void resetChildrenMinContentSize() override;

	private:
		void cellsAt(int col, int row, Cell& colCell, Cell& rowCell);

		osg::Vec2f minSizeAt(int col, int row);
		void actualOriginSizeAt(int col, int row, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size);

		void calculateMinSizeArray();
		void calculateActualSizeArray(osg::Vec2f area);

		static bool compareCells(Cell i, Cell j);

		int _numColumns;
		int _numRows;

		float _spacing;

		bool _calculatedMinSizeArray;
		bool _calculatedActualSizeArray;

		ChildRowColMap _childRowColMap;

		Cell* _rows;
		Cell* _columns;
	};
}