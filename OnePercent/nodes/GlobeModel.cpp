#include "GlobeModel.h"

#include "CountryOverlay.h"
#include "scripting/LuaConfig.h"

#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include <osgGaming/Helper.h>
#include <osgGaming/ResourceManager.h>
#include <osgGaming/TextureFactory.h>

#include <QString>

namespace onep
{
  struct GlobeModel::Impl
  {
    Impl(osgGaming::Injector& injector, GlobeModel* b)
      : base(b)
      , configManager(injector.inject<LuaConfig>())
      , resourceManager(injector.inject<osgGaming::ResourceManager>())
      , textureFactory(injector.inject<osgGaming::TextureFactory>())
      , countryOverlay(injector.inject<CountryOverlay>())
    {
      base->addChild(countryOverlay);
    }

    void makeEarthModel()
    {
      // planet geometry
      osg::ref_ptr<Node> earth = createPlanetGeode(0);
      osgGaming::generateTangentAndBinormal(earth);

      // material
      osg::ref_ptr<osg::Material> material = new osg::Material();
      material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.1, 0.1, 0.1, 1.0));
      material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4f(1.0, 1.0, 1.0, 1.0));
      material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.5, 0.5, 0.5, 1.0));
      material->setShininess(osg::Material::FRONT_AND_BACK, 16);
      material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.0, 0.0, 0.0, 1.0));

      // shader
      osg::ref_ptr<osg::Program> pgm = new osg::Program();

      osg::ref_ptr<osg::Shader> vertShader = resourceManager->loadShader("./GameData/shaders/globe.vert", osg::Shader::VERTEX);
      osg::ref_ptr<osg::Shader> fragShader = resourceManager->loadShader("./GameData/shaders/globe.frag", osg::Shader::FRAGMENT);

      pgm->addShader(vertShader);
      pgm->addShader(fragShader);

      pgm->addBindAttribLocation("tangent", 6);
      pgm->addBindAttribLocation("binormal", 7);

      // stateset
      osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
      stateSet->setAttribute(material);
      stateSet->setAttribute(pgm, osg::StateAttribute::ON);

      earth->setStateSet(stateSet);

      base->addChild(earth);
    }

    void makeCloudsModel()
    {
      // geometry
      cloudsTransform = new osg::PositionAttitudeTransform();

      osg::ref_ptr<osg::Geode> atmosphereGeode = createCloudsGeode();

      // material
      osg::ref_ptr<osg::Material> material = new osg::Material();
      material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.2, 0.2, 0.2, 1.0));
      material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4f(1.0, 1.0, 1.0, 1.0));
      material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.2, 0.2, 0.2, 1.0));
      material->setShininess(osg::Material::FRONT_AND_BACK, 32);
      material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4f(0.0, 0.0, 0.0, 1.0));

      // shader
      osg::ref_ptr<osg::Program> pgm = new osg::Program();

      osg::ref_ptr<osg::Shader> vertShader = resourceManager->loadShader("./GameData/shaders/clouds.vert", osg::Shader::VERTEX);
      osg::ref_ptr<osg::Shader> fragShader = resourceManager->loadShader("./GameData/shaders/clouds.frag", osg::Shader::FRAGMENT);

      pgm->addShader(vertShader);
      pgm->addShader(fragShader);

      uniformTime = new osg::Uniform("time", 0.0f);

      // stateset
      osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet();
      stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
      stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
      stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      stateSet->setRenderBinDetails(0, "RenderBin");
      stateSet->setAttribute(material);
      stateSet->setAttribute(pgm, osg::StateAttribute::ON);
      stateSet->addUniform(uniformTime);

      atmosphereGeode->setStateSet(stateSet);

      cloudsTransform->addChild(atmosphereGeode);
      base->addChild(cloudsTransform);
    }

    void makeAtmosphericScattering()
    {
      float earthRadius = configManager->getNumber<float>("earth.radius");
      float atmosphereHeight = configManager->getNumber<float>("earth.atmosphere_height");
      float scatteringDepth = configManager->getNumber<float>("earth.scattering_depth");
      float scatteringIntensity = configManager->getNumber<float>("earth.scattering_intensity");
      osg::Vec4f atmosphereColor = configManager->getVector<osg::Vec4f>("earth.atmosphere_color");

      // atmospheric scattering geometry
      scatteringQuad = new osgGaming::CameraAlignedQuad();

      // shader
      osg::ref_ptr<osg::Program> pgm = new osg::Program();

      osg::ref_ptr<osg::Shader> vertShader = resourceManager->loadShader("./GameData/shaders/atmosphere.vert", osg::Shader::VERTEX);
      osg::ref_ptr<osg::Shader> fragShader = resourceManager->loadShader("./GameData/shaders/atmosphere.frag", osg::Shader::FRAGMENT);

      pgm->addShader(vertShader);
      pgm->addShader(fragShader);

      double earthRad = pow(earthRadius * 0.9999, -2.0f);
      double atmosRad = pow(earthRadius + atmosphereHeight, -2.0f);

      osg::ref_ptr<osg::Uniform> uniformLightCol = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "light_col", 1);
      uniformLightCol->setElement(0, osg::Vec3f(0.0f, 0.0f, 0.0f));

      scatteringLightDirUniform = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "light_dir", 1);
      scatteringLightPosrUniform = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "light_posr", 1);

      // stateset
      osg::ref_ptr<osg::StateSet> stateSet = scatteringQuad->getOrCreateStateSet();
      stateSet->setAttributeAndModes(new osg::BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
      stateSet->addUniform(new osg::Uniform("planet_r", osg::Vec3f(earthRad, earthRad, earthRad)));
      stateSet->addUniform(new osg::Uniform("planet_R", osg::Vec3f(atmosRad, atmosRad, atmosRad)));
      stateSet->addUniform(new osg::Uniform("planet_h", atmosphereHeight));
      stateSet->addUniform(new osg::Uniform("view_depth", scatteringDepth));
      stateSet->addUniform(new osg::Uniform("lights", 1));
      stateSet->addUniform(scatteringLightDirUniform);
      stateSet->addUniform(uniformLightCol);
      stateSet->addUniform(scatteringLightPosrUniform);
      stateSet->addUniform(new osg::Uniform("B0", atmosphereColor * scatteringIntensity));
      stateSet->setAttribute(pgm, osg::StateAttribute::ON);

      base->addChild(scatteringQuad);
    }

    osg::ref_ptr<osg::Geode> createPlanetGeode(int textureDetailLevel)
    {
      osg::ref_ptr<osg::Geode> geode = new osg::Geode();

      std::string resolutionLevel0;
      std::string resolutionLevel1;
      int n, m;

      // texture detail level is always 0 for now
      switch (textureDetailLevel)
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

      int numStacks = configManager->getNumber<int>("earth.sphere_stacks");
      int numSlices = configManager->getNumber<int>("earth.sphere_slices");
      float radius = configManager->getNumber<float>("earth.radius");

      int stacksPerSurface = numStacks / m;
      int slicesPerSurface = numSlices / n;

      // Since the planet textures resolution exceeds the maximum supported size of 4096x4096,
      // we have to split them up and create separate meshes for each section.
      for (int y = 0; y < m; y++)
      {
        for (int x = 0; x < n; x++)
        {
          osg::ref_ptr<osg::Geometry> geo = createSphereSurfaceMesh(
            numStacks, numSlices, radius,
            y * stacksPerSurface,
            (y + 1) * stacksPerSurface - 1,
            x * slicesPerSurface,
            (x + 1) * slicesPerSurface - 1);

          osg::ref_ptr<osg::StateSet> stateSet = geo->getOrCreateStateSet();

          std::string colormapFilename  = QString("./GameData/textures/earth/color/%1/%2x%3.png").arg(resolutionLevel0.c_str()).arg(x).arg(y).toStdString();
          std::string nightmapFilename  = QString("./GameData/textures/earth/night/%1/%2x%3.png").arg(resolutionLevel0.c_str()).arg(x).arg(y).toStdString();
          std::string srbmapFilename    = QString("./GameData/textures/earth/speccitiesclouds/%1/%2x%3.png").arg(resolutionLevel1.c_str()).arg(x).arg(y).toStdString();
          std::string normalmapFilename = QString("./GameData/textures/earth/normal/%1/%2x%3.png").arg(resolutionLevel1.c_str()).arg(x).arg(y).toStdString();

          textureFactory->make()
            ->image(resourceManager->loadImage(colormapFilename))
            ->texLayer(0)
            ->uniform(stateSet, "colormap")
            ->assign(stateSet)
            ->build();

          textureFactory->make()
            ->image(resourceManager->loadImage(nightmapFilename))
            ->texLayer(1)
            ->uniform(stateSet, "nightmap")
            ->assign(stateSet)
            ->build();

          textureFactory->make()
            ->image(resourceManager->loadImage(srbmapFilename))
            ->texLayer(2)
            ->uniform(stateSet, "speccitiescloudsmap")
            ->assign(stateSet)
            ->build();

          textureFactory->make()
            ->image(resourceManager->loadImage(normalmapFilename))
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
      // pretty similar to createPlanetGeode()

      osg::ref_ptr<osg::Geode> geode = new osg::Geode();

      int n = 2;
      int m = 1;

      int stacks = configManager->getNumber<int>("earth.sphere_stacks");
      int slices = configManager->getNumber<int>("earth.sphere_slices");
      float radius = configManager->getNumber<float>("earth.radius");
      float cloudsHeight = configManager->getNumber<float>("earth.clouds_height");

      int stacksPerSurface = stacks / m;
      int slicesPerSurface = slices / n;

      for (int y = 0; y < m; y++)
      {
        for (int x = 0; x < n; x++)
        {
          osg::ref_ptr<osg::Geometry> geo = createSphereSurfaceMesh(
            stacks, slices, radius + cloudsHeight,
            y * stacksPerSurface,
            (y + 1) * stacksPerSurface - 1,
            x * slicesPerSurface,
            (x + 1) * slicesPerSurface - 1);

          osg::ref_ptr<osg::StateSet> stateSet = geo->getOrCreateStateSet();

          std::string colormapFilename = QString("./GameData/textures/earth/speccitiesclouds/8k/%1x%2.png").arg(x).arg(y).toStdString();

          textureFactory->make()
            ->image(resourceManager->loadImage(colormapFilename))
            ->texLayer(0)
            ->uniform(stateSet, "colormap")
            ->assign(stateSet)
            ->build();

          geode->addDrawable(geo);
        }
      }

      return geode;
    }

    /**
      * Creates the surface of a sphere with given radius and number of stacks and slices
      * within a given range [leftSlice, rightSlice] and [bottomStack, topStack]
      */ 
    osg::ref_ptr<osg::Geometry> createSphereSurfaceMesh(int stacks, int slices, double radius, int bottomStack, int topStack, int leftSlice, int rightSlice)
    {
      osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

      osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
      osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array();
      osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
      osg::ref_ptr<osg::DrawElementsUInt> triangles = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

      for (int slice = leftSlice; slice <= rightSlice + 1; slice++)
      {
        for (int stack = bottomStack; stack <= topStack + 1; stack++)
        {
          osg::Vec3 point = osgGaming::getVec3FromEuler(double(stack) * (C_PI / double(stacks)), 0.0, double(slice) * (2.0 * C_PI / double(slices)), osg::Vec3(0.0, 0.0, 1.0));

          vertices->push_back(point * radius);
          normals->push_back(point);

          double u = double(slice - leftSlice) / double(rightSlice - leftSlice + 1);
          double v = double(stack - bottomStack) / double(topStack - bottomStack + 1);

          texcoords->push_back(osg::Vec2(u, 1.0 - v));
        }
      }

      int nSlices = rightSlice - leftSlice + 1;
      int nStacks = topStack - bottomStack + 1;

      for (int slice = 0; slice < nSlices; slice++)
      {
        int slice_i = (nStacks + 1) * slice;
        int next_slice_i = slice_i + nStacks + 1;

        for (int stack = 0; stack < nStacks; stack++)
        {
          if (stack == 0 && bottomStack == 0)
          {
            triangles->push_back(slice_i);
            triangles->push_back(slice_i + 1);
            triangles->push_back(next_slice_i + 1);
          }
          else if (stack == nStacks - 1 && topStack == stacks - 1)
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

    osg::ref_ptr<LuaConfig> configManager;
    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<osgGaming::TextureFactory> textureFactory;

    float propSunDistance;
    float propSunRadiusMp2;
    float propEarthCloudsSpeed;
    float propEarthCloudsMorphSpeed;

    osg::ref_ptr<osg::Uniform> scatteringLightDirUniform;
    osg::ref_ptr<osg::Uniform> scatteringLightPosrUniform;

    osg::ref_ptr<osg::PositionAttitudeTransform> cloudsTransform;
    osg::ref_ptr<osg::Uniform> uniformTime;

    osg::ref_ptr<CountryOverlay> countryOverlay;

    osg::ref_ptr<osgGaming::CameraAlignedQuad> scatteringQuad;
  };

  GlobeModel::GlobeModel(osgGaming::Injector& injector)
    : osg::Group()
    , m(new Impl(injector, this))
  {
  }

  GlobeModel::~GlobeModel()
  {
  }

  void GlobeModel::makeGlobeModel()
  {
    m->propSunDistance = m->configManager->getNumber<float>("sun.distance");
    m->propSunRadiusMp2 = m->configManager->getNumber<float>("sun.radius_pm2");
    m->propEarthCloudsSpeed = m->configManager->getNumber<float>("earth.clouds_speed");
    m->propEarthCloudsMorphSpeed = m->configManager->getNumber<float>("earth.clouds_morph_speed");

    m->makeEarthModel();
    m->makeCloudsModel();
    m->makeAtmosphericScattering();
  }

  osgGaming::CameraAlignedQuad::Ptr GlobeModel::getScatteringQuad()
  {
    return m->scatteringQuad;
  }

  void GlobeModel::updateLightDir(const osg::Vec3f& direction)
  {
    m->scatteringLightDirUniform->setElement(0, direction);
    osg::Vec3f position = -direction * m->propSunDistance;

    m->scatteringLightPosrUniform->setElement(0, osg::Vec4f(position.x(), position.y(), position.z(), m->propSunRadiusMp2));
  }

  void GlobeModel::updateClouds(float time)
  {
    osg::Quat quat = osgGaming::getQuatFromEuler(0.0, 0.0, fmodf(time * m->propEarthCloudsSpeed, C_2PI));
    m->cloudsTransform->setAttitude(quat);

    m->uniformTime->set(time * m->propEarthCloudsMorphSpeed);
  }

}
