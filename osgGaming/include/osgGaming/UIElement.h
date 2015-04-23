#pragma once

#include <vector>

#include <osg/MatrixTransform>

namespace osgGaming
{
	class UIElement : public osg::MatrixTransform
	{
	public:
		typedef std::vector<osg::ref_ptr<UIElement>> UIElementList;

		typedef enum _horizontalAlignment
		{
			LEFT,
			CENTER,
			RIGHT,
			H_STRETCH
		} HorizontalAlignment;

		typedef enum _verticalAlignment
		{
			TOP,
			MIDDLE,
			BOTTOM,
			V_STRETCH
		} VerticalAlignment;

		UIElement();

		osg::Vec2f getOrigin();
		osg::Vec2f getSize();

		float getWidth();
		float getHeight();

		osg::Vec4f getPadding();
		osg::Vec4f getMargin();

		HorizontalAlignment getHorizontalAlignment();
		VerticalAlignment getVerticalAlignment();

		virtual void getOriginSizeForChildInArea(unsigned int i, osg::Vec2f area, osg::Vec2f& origin, osg::Vec2f& size);

		UIElementList getUIChildren();
		unsigned int getNumUIChildren();

		void setOrigin(osg::Vec2f origin);
		void setSize(osg::Vec2f size);

		void setWidth(float width);
		void setHeight(float height);

		void setPadding(osg::Vec4f padding);
		void setMargin(osg::Vec4f margin);

		void setPadding(float padding);
		void setMargin(float margin);

		void setHorizontalAlignment(HorizontalAlignment alignment);
		void setVerticalAlignment(VerticalAlignment alignment);

		virtual void updatedContentOriginSize(osg::Vec2f origin, osg::Vec2f size);

	//protected:
		osg::ref_ptr<osg::Group> getVisualGroup();

	private:
		void updateVisualGroup();

		osg::Vec2f _origin;
		osg::Vec2f _size;

		float _width;
		float _height;

		osg::Vec4f _padding;
		osg::Vec4f _margin;

		HorizontalAlignment _horizontalAlignment;
		VerticalAlignment _verticalAlignment;

		osg::ref_ptr<osg::MatrixTransform> _visualGroup;
	};
}