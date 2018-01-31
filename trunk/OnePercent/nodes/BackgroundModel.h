#pragma once

#include <memory>

#include <osgGaming/Injector.h>

#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventHandler>

namespace onep
{
	class BackgroundModel : public osgGA::GUIEventHandler
	{
	public:
		BackgroundModel(osgGaming::Injector& injector);
    ~BackgroundModel();

    void loadStars(std::string filename);

		osg::ref_ptr<osg::PositionAttitudeTransform> getTransform();
		osg::ref_ptr<osg::PositionAttitudeTransform> getSunTransform();
		osg::ref_ptr<osg::PositionAttitudeTransform> getSunGlowTransform();

		void updateResolutionHeight(float height);

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}