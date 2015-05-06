#pragma once

#include <osg/Geometry>
#include <osg/Program>
#include <osg/Uniform>

#include <osgGaming/TransformableCameraManipulator.h>

#include <string>

namespace onep
{
	class GlobeModel : public osg::Group
	{
	public:
		static const double EARTH_RADIUS;
		static const double CLOUDS_HEIGHT;
		static const double ATMOSPHERE_HEIGHT;
		static const double SCATTERING_DEPTH;
		static const double SCATTERING_INTENSITY;
		static const osg::Vec4f ATMOSPHERE_COLOR;
		static const int SPHERE_STACKS;
		static const int SPHERE_SLICES;
		static const double SUN_DISTANCE;
		static const double SUN_RADIUS_PM2;

		GlobeModel(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);

		void updateLightDirection(osg::Vec3f direction);
		void updateClouds(double simTime);

	private:
		void makeEarthModel();
		void makeCloudsModel();
		void makeAtmosphericScattering(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);

		osg::ref_ptr<osg::Geode> createPlanetGeode(int textureResolution);
		osg::ref_ptr<osg::Geode> createCloudsGeode();

		osg::ref_ptr<osg::Geometry> createSphereSegmentMesh(int stacks, int slices, double radius, int firstStack, int lastStack, int firstSlice, int lastSlice);

		osg::ref_ptr<osg::Uniform> _scatteringLightDirUniform;
		osg::ref_ptr<osg::Uniform> _scatteringLightPosrUniform;

		osg::ref_ptr<osg::PositionAttitudeTransform> _cloudsTransform;
	};
}