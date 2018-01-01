#pragma once

#include "core/Globals.h"

#include <osg/Geometry>

#include <osgGaming/TransformableCameraManipulator.h>

#include "CountryMesh.h"

#include "data/CountriesMap.h"
#include "simulation/CountryData.h"
#include "nodes/BoundariesMesh.h"

namespace onep
{
	class GlobeModel : public osg::Group
	{
	public:
    typedef osg::ref_ptr<GlobeModel> Ptr;

		GlobeModel(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm);
    ~GlobeModel();

		void updateLightDirection(osg::Vec3f direction);
		void updateClouds(float day);

    void clearHighlightedCountries();
		void setSelectedCountry(int countryId);
    void setHighlightedSkillBranch(BranchType type);

    void setHoveredCountry(CountryMesh::Ptr countryMesh);

    void setCountriesMap(CountriesMap::Ptr countriesMap);

    // void setCountryVisibility(CountryMesh::Ptr countryMesh, bool visibility);
    void setAllCountriesVisibility(bool visibility);

    void addCountry(int id, CountryData::Ptr countryData, osg::ref_ptr<osg::DrawElementsUInt> triangles, osg::ref_ptr<osg::Texture2D> distanceTexture, CountryMesh::BorderIdMap& neighborBorders);
    CountryMesh::Map& getCountryMeshs();
    CountriesMap::Ptr getCountriesMap();
    BoundariesMesh::Ptr getBoundariesMesh();

    CountryMesh::Ptr getSelectedCountryMesh();
    CountryMesh::Ptr getCountryMesh(int id);
    CountryMesh::Ptr getCountryMesh(osg::Vec2f coord);
    int getCountryId(osg::Vec2f coord);
    std::string getCountryName(osg::Vec2f coord);

    int getSelectedCountryId();
    osgGaming::Observable<int>::Ptr getSelectedCountryIdObservable();

	private:
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
