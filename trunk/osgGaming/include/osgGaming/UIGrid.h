#pragma once

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
    typedef osg::ref_ptr<UIGrid> Ptr;

		UIGrid();

		virtual void getOriginSizeForLocationInArea(ColRow location, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size) override;

		void setSpacing(float spacing);

		osg::ref_ptr<UICells> getRows();
		osg::ref_ptr<UICells> getColumns();

	protected:
		virtual osg::Vec2f calculateMinContentSize() override;

	private:
		float _spacing;

		osg::ref_ptr<UICells> _rows;
		osg::ref_ptr<UICells> _columns;
	};
}