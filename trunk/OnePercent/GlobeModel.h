#pragma once

#include "Globals.h"

#include <osg/Geometry>
#include <osg/Uniform>

#include <osgGaming/TransformableCameraManipulator.h>
#include <osgGaming/Property.h>

namespace onep
{
	class GlobeModel : public osg::Group
	{
	public:
		GlobeModel(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);

		void updateLightDirection(osg::Vec3f direction);
		void updateClouds(float day);

		void setSelectedCountry(int countryId);

    void addCountryTriangles(int id, osg::ref_ptr<osg::DrawElementsUInt> triangles);

	private:
    typedef struct _countrySurface
    {
      osg::ref_ptr<osg::Switch> switchNode;
    } CountrySurface;

    typedef std::map<int, CountrySurface> CountrySurfaceMap;

		osgGaming::Property<float, Param_SunDistanceName> _paramSunDistance;
		osgGaming::Property<float, Param_SunRadiusPm2Name> _paramSunRadiusMp2;

		osgGaming::Property<float, Param_EarthCloudsSpeedName> _paramEarthCloudsSpeed;
		osgGaming::Property<float, Param_EarthCloudsMorphSpeedName> _paramEarthCloudsMorphSpeed;

		void makeEarthModel();
		void makeCloudsModel();
    void makeBoundariesModel();
		void makeAtmosphericScattering(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);

		osg::ref_ptr<osg::Geode> createPlanetGeode(int textureResolution);
		osg::ref_ptr<osg::Geode> createCloudsGeode();

		osg::ref_ptr<osg::Geometry> createSphereSegmentMesh(int stacks, int slices, double radius, int firstStack, int lastStack, int firstSlice, int lastSlice);

		osg::ref_ptr<osg::Uniform> _scatteringLightDirUniform;
		osg::ref_ptr<osg::Uniform> _scatteringLightPosrUniform;

		osg::ref_ptr<osg::Uniform> _uniformSelectedCountry;

		osg::ref_ptr<osg::PositionAttitudeTransform> _cloudsTransform;
		osg::ref_ptr<osg::Uniform> _uniformTime;

    osg::ref_ptr<osg::Vec3Array> _countriesVertices;
    osg::ref_ptr<Group> _countrySurfacesGroup;
    CountrySurfaceMap _countrySurfaces;

    int _enabledSurface;
	};
}