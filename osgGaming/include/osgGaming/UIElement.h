#pragma once

#include <vector>

#include <osg/MatrixTransform>

namespace osgGaming
{
	class UIElement : public osg::MatrixTransform
	{
	public:
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

		virtual bool addChild(Node* child) override;
		virtual bool insertChild(unsigned int index, Node* child) override;
		virtual bool replaceChild(Node* origChild, Node* newChild) override;
		virtual bool setChild(unsigned int i, Node *node) override;

		osg::Vec2f getOrigin();
		osg::Vec2f getSize();
		osg::Vec2f getContentSize();

		float getWidth();
		float getHeight();

		osg::Vec4f getPadding();
		osg::Vec4f getMargin();

		HorizontalAlignment getHorizontalAlignment();
		VerticalAlignment getVerticalAlignment();

		osg::Vec2f getMinContentSize();
		osg::Vec2f getMinSize();

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

		void resetMinContentSize();

	//protected:
		osg::ref_ptr<osg::Group> getVisualGroup();
	protected:
		virtual osg::Vec2f calculateMinContentSize();

		virtual void onResetMinContentSize();

	private:
		void updateVisualGroup();

		osg::Vec2f _origin;
		osg::Vec2f _size;

		osg::Vec2f _minContentSize;
		bool _calculatedMinContentSize;

		float _width;
		float _height;

		osg::Vec4f _padding;
		osg::Vec4f _margin;

		HorizontalAlignment _horizontalAlignment;
		VerticalAlignment _verticalAlignment;

		osg::ref_ptr<osg::MatrixTransform> _visualGroup;
	};
}