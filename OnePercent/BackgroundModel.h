#pragma once

#include <string>

#include <osg/PositionAttitudeTransform>
#include <osg/Point>
#include <osgGA/GUIEventHandler>

namespace onep
{
	class BackgroundModel : public osgGA::GUIEventHandler
	{
	public:
		BackgroundModel(std::string starsFilename);

		osg::ref_ptr<osg::PositionAttitudeTransform> getTransform();

		void updateResolutionHeight(float height);

		virtual bool BackgroundModel::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

	private:
		void makeStars(std::string starsFilename);

		osg::ref_ptr<osg::PositionAttitudeTransform> _transform;
		osg::ref_ptr<osg::Point> _point;
	};
}