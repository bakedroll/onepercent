#pragma once

#include <osg/PositionAttitudeTransform>
#include <osg/Point>
#include <osgGA/GUIEventHandler>

namespace onep
{
	class BackgroundModel : public osgGA::GUIEventHandler
	{
	public:
		BackgroundModel();

		osg::ref_ptr<osg::PositionAttitudeTransform> getTransform();
		osg::ref_ptr<osg::PositionAttitudeTransform> getSunTransform();
		osg::ref_ptr<osg::PositionAttitudeTransform> getSunGlowTransform();

		void updateResolutionHeight(float height);

		virtual bool BackgroundModel::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

	private:
		void makeStars();
		void makeSun();

		osg::ref_ptr<osg::PositionAttitudeTransform> _transform;
		osg::ref_ptr<osg::PositionAttitudeTransform> _sunTransform;
		osg::ref_ptr<osg::PositionAttitudeTransform> _sunGlowTransform;
		osg::ref_ptr<osg::Point> _point;
	};
}