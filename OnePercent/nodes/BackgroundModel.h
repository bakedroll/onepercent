#pragma once

#include <memory>

#include <osgHelper/ioc/Injector.h>

#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventHandler>

namespace onep
{
	class BackgroundModel : public osg::PositionAttitudeTransform
	{
	public:
		BackgroundModel(osgHelper::ioc::Injector& injector);
    ~BackgroundModel();

    void loadStars(std::string filename);

		osg::ref_ptr<osg::PositionAttitudeTransform> getSunTransform();
		osg::ref_ptr<osg::PositionAttitudeTransform> getSunGlowTransform();

		void updateResolutionHeight(float height);

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}