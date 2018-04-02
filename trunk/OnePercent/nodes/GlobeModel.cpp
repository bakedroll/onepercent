#include "GlobeModel.h"

#include "BoundariesMesh.h"
#include "CountryOverlay.h"

#include "core/Globals.h"

#include <osg/Geode>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

#include <osgGaming/ResourceManager.h>
#include <osgGaming/PropertiesManager.h>
#include <osgGaming/Helper.h>
#include <osgGaming/TextureFactory.h>
#include <osg/Material>
#include <osg/BlendFunc>

namespace onep
{
  struct GlobeModel::Impl
  {
    Impl(osgGaming::Injector& injector, GlobeModel* b)
      : base(b)
      , resourceManager(injector.inject<osgGaming::ResourceManager>())
      , propertiesManager(injector.inject<osgGaming::PropertiesManager>())
      , textureFactory(injector.inject<osgGaming::TextureFactory>())
      , boundariesMesh(injector.inject<BoundariesMesh>())
      , countryOverlay(injector.inject<CountryOverlay>())
    {
      base->addChild(boundariesMesh);
      base->addChild(countryOverlay);
    }

    void makeEarthModel()
    {
      // planet geometry
      osg::ref_ptr<Node> earth = createPlanetGeode(0);
      osgGaming::generateTangentAndBinormal(earth);

      // stateset
      osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

      osg::ref_ptr<osg::Material> material = new osg::Material();
      material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.1, 0.1, 0.1, 1.0));
      material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4f(1.0, 1.0, 1.0, 1.0));
      material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.5, 0.5, 0.5, 1.0));
      material->setShininess(osg::Material::FRONT_AND_BACK, 16);
      material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.0, 0.0, 0.0, 1.0));

      stateSet->setAttribute(material);

      // shader
      osg::ref_ptr<osg::Program> pgm = new osg::Program();

      osg::ref_ptr<osg::Shader> vert_shader = resourceManager->loadShader("./GameData/shaders/globe.vert", osg::Shader::VERTEX);
      osg::ref_ptr<osg::Shader> frag_shader = resourceManager->loadShader("./GameData/shaders/globe.frag", osg::Shader::FRAGMENT);

      pgm->addShader(vert_shader);
      pgm->addShader(frag_shader);

      pgm->addBindAttribLocation("tangent", 6);
      pgm->addBindAttribLocation("binormal", 7);

      stateSet->setAttribute(pgm, osg::StateAttribute::ON);
      //stateSet->setRenderBinDetails(0, "RenderBin");

      earth->setStateSet(stateSet);

      base->addChild(earth);
    }

    void makeCloudsModel()
    {
      // geometry
      cloudsTransform = new osg::PositionAttitudeTransform();

      osg::ref_ptr<osg::Geode> atmosphere_geode = createCloudsGeode();

      // stateset
      osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();

      osg::ref_ptr<osg::Material> material = new osg::Material();
      material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.2, 0.2, 0.2, 1.0));
      material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4f(1.0, 1.0, 1.0, 1.0));
      material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.2, 0.2, 0.2, 1.0));
      material->setShininess(osg::Material::FRONT_AND_BACK, 32);
      material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.0, 0.0, 0.0, 1.0));

      stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
      stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      stateSet->setRenderBinDetails(0, "RenderBin");
      stateSet->setAttribute(material);

      // shader
      osg::ref_ptr<osg::Program> pgm = new osg::Program();

      osg::ref_ptr<osg::Shader> vert_shader = resourceManager->loadShader("./GameData/shaders/clouds.vert", osg::Shader::VERTEX);
      osg::ref_ptr<osg::Shader> frag_shader = resourceManager->loadShader("./GameData/shaders/clouds.frag", osg::Shader::FRAGMENT);

      pgm->addShader(vert_shader);
      pgm->addShader(frag_shader);

      uniformTime = new osg::Uniform("time", 0.0f);

      stateSet->setAttribute(pgm, osg::StateAttribute::ON);
      stateSet->addUniform(uniformTime);

      atmosphere_geode->setStateSet(stateSet);

      cloudsTransform->addChild(atmosphere_geode);
      base->addChild(cloudsTransform);
    }

    void makeAtmosphericScattering(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm)
    {
      float earthRadius = propertiesManager->getValue<float>(Param_EarthRadiusName);
      float atmosphereHeight = propertiesManager->getValue<float>(Param_EarthAtmosphereHeightName);
      float scatteringDepth = propertiesManager->getValue<float>(Param_EarthScatteringDepthName);
      float scatteringIntensity = propertiesManager->getValue<float>(Param_EarthScatteringIntensityName);
      osg::Vec4f atmosphereColor = propertiesManager->getValue<osg::Vec4f>(Param_EarthAtmosphereColorName);

      // atmospheric scattering geometry
      osg::ref_ptr<osgGaming::CameraAlignedQuad> caq = new osgGaming::CameraAlignedQuad();

      // shader
      osg::ref_ptr<osg::StateSet> stateSet = caq->getOrCreateStateSet();
      stateSet->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);

      osg::ref_ptr<osg::Program> pgm = new osg::Program();

      osg::ref_ptr<osg::Shader> vert_shader = resourceManager->loadShader("./GameData/shaders/atmosphere.vert", osg::Shader::VERTEX);
      osg::ref_ptr<osg::Shader> frag_shader = resourceManager->loadShader("./GameData/shaders/atmosphere.frag", osg::Shader::FRAGMENT);

      pgm->addShader(vert_shader);
      pgm->addShader(frag_shader);

      double earth_rad = pow(earthRadius * 0.9999, -2.0f);
      double atmos_rad = pow(earthRadius + atmosphereHeight, -2.0f);

      stateSet->addUniform(new osg::Uniform("planet_r", osg::Vec3f(earth_rad, earth_rad, earth_rad)));
      stateSet->addUniform(new osg::Uniform("planet_R", osg::Vec3f(atmos_rad, atmos_rad, atmos_rad)));
      stateSet->addUniform(new osg::Uniform("planet_h", atmosphereHeight));
      stateSet->addUniform(new osg::Uniform("view_depth", scatteringDepth));

      scatteringLightDirUniform = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "light_dir", 1);
      osg::ref_ptr<osg::Uniform> light_col_uniform = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "light_col", 1);
      scatteringLightPosrUniform = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "light_posr", 1);

      //light_col_uniform->setElement(0, Vec3f(1.0f, 1.0f, 1.0f));
      light_col_uniform->setElement(0, osg::Vec3f(0.0f, 0.0f, 0.0f));

      stateSet->addUniform(new osg::Uniform("lights", 1));
      stateSet->addUniform(scatteringLightDirUniform);
      stateSet->addUniform(light_col_uniform);
      stateSet->addUniform(scatteringLightPosrUniform);

      stateSet->addUniform(new osg::Uniform("B0", atmosphereColor * scatteringIntensity));

      stateSet->setAttribute(pgm, osg::StateAttribute::ON);

      base->addChild(caq);
      tcm->addCameraAlignedQuad(caq);
    }

    osg::ref_ptr<osg::Geode> createPlanetGeode(int textureResolution)
    {
      osg::ref_ptr<osg::Geode> geode = new osg::Geode();

      std::string resolutionLevel0;
      std::string resolutionLevel1;
      int n, m;

      switch (textureResolution)
      {
      default:
      case 0:
        n = 2;
        m = 1;
        resolutionLevel0 = "8k";
        resolutionLevel1 = "8k";
        break;
      case 1:
        n = 4;
        m = 2;
        resolutionLevel0 = "16k";
        resolutionLevel1 = "8k_4x2";
        break;
      };

      int stacks = propertiesManager->getValue<int>(Param_EarthSphereStacksName);
      int slices = propertiesManager->getValue<int>(Param_EarthSphereSlicesName);
      float radius = propertiesManager->getValue<float>(Param_EarthRadiusName);

      int stacksPerSegment = stacks / m;
      int slicesPerSegment = slices / n;

      for (int y = 0; y < m; y++)
      {
        for (int x = 0; x < n; x++)
        {
          osg::ref_ptr<osg::Geometry> geo = createSphereSegmentMesh(
            stacks, slices, radius,
            y * stacksPerSegment,
            (y + 1) * stacksPerSegment - 1,
            x * slicesPerSegment,
            (x + 1) * slicesPerSegment - 1);

          osg::ref_ptr<osg::StateSet> stateSet = geo->getOrCreateStateSet();

          char colormap_file[128];
          char nightmap_file[128];
          char specreliefcitiesboundariesmap_file[128];
          char normalmap_file[128];

          sprintf(colormap_file, "./GameData/textures/earth/color/%s/%dx%d.png", resolutionLevel0.c_str(), x, y);
          sprintf(nightmap_file, "./GameData/textures/earth/night/%s/%dx%d.png", resolutionLevel0.c_str(), x, y);
          sprintf(specreliefcitiesboundariesmap_file, "./GameData/textures/earth/speccitiesclouds/%s/%dx%d.png", resolutionLevel1.c_str(), x, y);
          sprintf(normalmap_file, "./GameData/textures/earth/normal/%s/%dx%d.png", resolutionLevel1.c_str(), x, y);

          textureFactory->make()
            ->image(resourceManager->loadImage(colormap_file))
            ->texLayer(0)
            ->uniform(stateSet, "colormap")
            ->assign(stateSet)
            ->build();

          textureFactory->make()
            ->image(resourceManager->loadImage(nightmap_file))
            ->texLayer(1)
            ->uniform(stateSet, "nightmap")
            ->assign(stateSet)
            ->build();

          textureFactory->make()
            ->image(resourceManager->loadImage(specreliefcitiesboundariesmap_file))
            ->texLayer(2)
            ->uniform(stateSet, "speccitiescloudsmap")
            ->assign(stateSet)
            ->build();

          textureFactory->make()
            ->image(resourceManager->loadImage(normalmap_file))
            ->texLayer(3)
            ->uniform(stateSet, "normalmap")
            ->assign(stateSet)
            ->build();

          geode->addDrawable(geo);
        }
      }

      return geode;
    }

    osg::ref_ptr<osg::Geode> createCloudsGeode()
    {
      osg::ref_ptr<osg::Geode> geode = new osg::Geode();

      int n = 2;
      int m = 1;

      int stacks = propertiesManager->getValue<int>(Param_EarthSphereStacksName);
      int slices = propertiesManager->getValue<int>(Param_EarthSphereSlicesName);
      float radius = propertiesManager->getValue<float>(Param_EarthRadiusName);
      float cloudsHeight = propertiesManager->getValue<float>(Param_EarthCloudsHeightName);

      int stacksPerSegment = stacks / m;
      int slicesPerSegment = slices / n;

      for (int y = 0; y < m; y++)
      {
        for (int x = 0; x < n; x++)
        {
          osg::ref_ptr<osg::Geometry> geo = createSphereSegmentMesh(
            stacks, slices, radius + cloudsHeight,
            y * stacksPerSegment,
            (y + 1) * stacksPerSegment - 1,
            x * slicesPerSegment,
            (x + 1) * slicesPerSegment - 1);

          osg::ref_ptr<osg::StateSet> stateSet = geo->getOrCreateStateSet();

          char colormap_file[128];
          sprintf(colormap_file, "./GameData/textures/earth/speccitiesclouds/8k/%dx%d.png", x, y);

          textureFactory->make()
            ->image(resourceManager->loadImage(colormap_file))
            ->texLayer(0)
            ->uniform(stateSet, "colormap")
            ->assign(stateSet)
            ->build();

          geode->addDrawable(geo);
        }
      }

      return geode;
    }

    osg::ref_ptr<osg::Geometry> createSphereSegmentMesh(int stacks, int slices, double radius, int firstStack, int lastStack, int firstSlice, int lastSlice)
    {
      osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

      osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
      osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
      osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
      osg::ref_ptr<osg::DrawElementsUInt> triangles = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

      for (int slice = firstSlice; slice <= lastSlice + 1; slice++)
      {
        for (int stack = firstStack; stack <= lastStack + 1; stack++)
        {
          osg::Vec3 point = osgGaming::getVec3FromEuler(double(stack) * (C_PI / double(stacks)), 0.0, double(slice) * (2.0 * C_PI / double(slices)), osg::Vec3(0.0, 0.0, 1.0));

          vertices->push_back(point * radius);
          normals->push_back(point);

          double u = double(slice - firstSlice) / double(lastSlice - firstSlice + 1);
          double v = double(stack - firstStack) / double(lastStack - firstStack + 1);

          texcoords->push_back(osg::Vec2(u, 1.0 - v));
        }
      }

      int nSlices = lastSlice - firstSlice + 1;
      int nStacks = lastStack - firstStack + 1;

      for (int slice = 0; slice < nSlices; slice++)
      {
        int slice_i = (nStacks + 1) * slice;
        int next_slice_i = slice_i + nStacks + 1;

        for (int stack = 0; stack < nStacks; stack++)
        {
          if (stack == 0 && firstStack == 0)
          {
            triangles->push_back(slice_i);
            triangles->push_back(slice_i + 1);
            triangles->push_back(next_slice_i + 1);
          }
          else if (stack == nStacks - 1 && lastStack == stacks - 1)
          {
            triangles->push_back(slice_i + nStacks);
            triangles->push_back(next_slice_i + nStacks - 1);
            triangles->push_back(slice_i + nStacks - 1);
          }
          else
          {
            triangles->push_back(slice_i + stack);
            triangles->push_back(slice_i + stack + 1);
            triangles->push_back(next_slice_i + stack);

            triangles->push_back(next_slice_i + stack);
            triangles->push_back(slice_i + stack + 1);
            triangles->push_back(next_slice_i + stack + 1);
          }
        }
      }

      osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
      colors->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));

      geometry->setVertexArray(vertices);
      geometry->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
      geometry->setTexCoordArray(0, texcoords, osg::Array::BIND_PER_VERTEX);
      geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
      geometry->addPrimitiveSet(triangles);

      return geometry;
    }

    GlobeModel* base;

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<osgGaming::PropertiesManager> propertiesManager;
    osg::ref_ptr<osgGaming::TextureFactory> textureFactory;

    float paramSunDistance;
    float paramSunRadiusMp2;

    float paramEarthCloudsSpeed;
    float paramEarthCloudsMorphSpeed;

    osg::ref_ptr<osg::Uniform> scatteringLightDirUniform;
    osg::ref_ptr<osg::Uniform> scatteringLightPosrUniform;

    osg::ref_ptr<osg::PositionAttitudeTransform> cloudsTransform;
    osg::ref_ptr<osg::Uniform> uniformTime;

    osg::ref_ptr<BoundariesMesh> boundariesMesh;
    osg::ref_ptr<CountryOverlay> countryOverlay;
  };

  GlobeModel::GlobeModel(osgGaming::Injector& injector)
    : osg::Group()
    , m(new Impl(injector, this))
  {
  }

  GlobeModel::~GlobeModel()
  {
  }

  void GlobeModel::loadFromDisk(osg::ref_ptr<osgGaming::TransformableCameraManipulator> tcm)
  {
    m->paramSunDistance = m->propertiesManager->getValue<float>(Param_SunDistanceName);
    m->paramSunRadiusMp2 = m->propertiesManager->getValue<float>(Param_SunRadiusPm2Name);
    m->paramEarthCloudsSpeed = m->propertiesManager->getValue<float>(Param_EarthCloudsSpeedName);
    m->paramEarthCloudsMorphSpeed = m->propertiesManager->getValue<float>(Param_EarthCloudsMorphSpeedName);

    m->makeEarthModel();
    m->makeCloudsModel();
    m->makeAtmosphericScattering(tcm);
  }

  void GlobeModel::updateLightDirection(osg::Vec3f direction)
  {
    m->scatteringLightDirUniform->setElement(0, direction);
    osg::Vec3f position = -direction * m->paramSunDistance;

    m->scatteringLightPosrUniform->setElement(0, osg::Vec4f(position.x(), position.y(), position.z(), m->paramSunRadiusMp2));
  }

  void GlobeModel::updateClouds(float day)
  {
    osg::Quat quat = osgGaming::getQuatFromEuler(0.0, 0.0, fmodf(day * m->paramEarthCloudsSpeed, C_2PI));
    m->cloudsTransform->setAttitude(quat);

    m->uniformTime->set(day * m->paramEarthCloudsMorphSpeed);
  }

}
