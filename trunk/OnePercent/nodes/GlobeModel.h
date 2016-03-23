#pragma once

#include "core/Globals.h"

#include <osg/Geometry>
#include <osg/Uniform>

#include <osgGaming/TransformableCameraManipulator.h>
#include <osgGaming/Property.h>

#include "CountryMesh.h"

#include "data/CountriesMap.h"
#include "simulation/CountryData.h"

namespace onep
{
	class GlobeModel : public osg::Group
	{
	public:
    typedef osg::ref_ptr<GlobeModel> Ptr;

		GlobeModel(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);

		void updateLightDirection(osg::Vec3f direction);
		void updateClouds(float day);

    void clearHighlightedCountries();
		void setSelectedCountry(int countryId);
    void setHighlightedSkillBranch(BranchType type);

    void setCountriesMap(CountriesMap::Ptr countriesMap);

    void addCountry(int id, CountryData::Ptr countryData, osg::ref_ptr<osg::DrawElementsUInt> triangles);
    CountryMesh::Map& getCountryMeshs();
    CountriesMap::Ptr getCountriesMap();

    CountryMesh::Ptr getSelectedCountryMesh();
    CountryMesh::Ptr getCountryMesh(int id);
    CountryMesh::Ptr getCountryMesh(osg::Vec2f coord);
    int getCountryId(osg::Vec2f coord);
    std::string getCountryName(osg::Vec2f coord);

    int getSelectedCountryId();
    osgGaming::Observable<int>::Ptr getSelectedCountryIdObservable();

	private:
		osgGaming::Property<float, Param_SunDistanceName> m_paramSunDistance;
		osgGaming::Property<float, Param_SunRadiusPm2Name> m_paramSunRadiusMp2;

		osgGaming::Property<float, Param_EarthCloudsSpeedName> m_paramEarthCloudsSpeed;
		osgGaming::Property<float, Param_EarthCloudsMorphSpeedName> m_paramEarthCloudsMorphSpeed;

		void makeEarthModel();
		void makeCloudsModel();
    void makeBoundariesModel();
		void makeAtmosphericScattering(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);

    void addHighlightedCountry(CountryMesh::Ptr mesh, CountryMesh::ColorMode mode);

		osg::ref_ptr<osg::Geode> createPlanetGeode(int textureResolution);
		osg::ref_ptr<osg::Geode> createCloudsGeode();

		osg::ref_ptr<osg::Geometry> createSphereSegmentMesh(int stacks, int slices, double radius, int firstStack, int lastStack, int firstSlice, int lastSlice);

		osg::ref_ptr<osg::Uniform> m_scatteringLightDirUniform;
		osg::ref_ptr<osg::Uniform> m_scatteringLightPosrUniform;

		osg::ref_ptr<osg::PositionAttitudeTransform> m_cloudsTransform;
		osg::ref_ptr<osg::Uniform> m_uniformTime;

    osg::ref_ptr<osg::Vec3Array> m_countriesVertices;
    osg::ref_ptr<osg::Switch> m_countrySurfacesSwitch;
    CountryMesh::Map m_countryMeshs;
    CountriesMap::Ptr m_countriesMap;

    CountryMesh::List m_visibleCountryMeshs;
    osgGaming::Observable<int>::Ptr m_oSelectedCountryId;

    BranchType m_highlightedBranch;
	};
}