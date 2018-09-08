#include "CountryNameOverlay.h"

#include "simulation/CountriesContainer.h"
#include "simulation/Country.h"

#include <osg/Billboard>
#include <osg/Switch>

#include <osgGaming/Helper.h>
#include <osgGaming/ResourceManager.h>

namespace onep
{
  struct CountryNameOverlay::Impl
  {
    Impl(osgGaming::Injector& injector)
      : resourceManager(injector.inject<osgGaming::ResourceManager>())
      , configManager(injector.inject<LuaConfig>())
      , countriesContainer(injector.inject<CountriesContainer>())
      , enabled(true)
    {
      switchNode = new osg::Switch();
      switchNode->setNewChildDefaultValue(enabled);

      switchNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    }

    osg::ref_ptr<osgGaming::ResourceManager> resourceManager;
    osg::ref_ptr<LuaConfig> configManager;
    osg::ref_ptr<CountriesContainer> countriesContainer;

    osg::ref_ptr<osg::Switch> switchNode;
    const CountryNode::Map* countryMap;

    bool enabled;
  };

  CountryNameOverlay::CountryNameOverlay(osgGaming::Injector& injector)
    : PositionAttitudeTransform()
    , m(new Impl(injector))
  {
    addChild(m->switchNode);
    setScale(osg::Vec3f(0.01, 0.01, 0.01));
  }

  CountryNameOverlay::~CountryNameOverlay()
  {
  }

  bool CountryNameOverlay::getEnabled()
  {
    return m->enabled;
  }

  void CountryNameOverlay::setCountryMap(const CountryNode::Map& countryMap)
  {
    osg::ref_ptr<osgText::Font> font = m->resourceManager->loadFont("./GameData/fonts/coolvetica rg.ttf");

    float earthRadius = m->configManager->getNumber<float>("earth.radius");

    m->countryMap = &countryMap;

    osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
    billboard->setMode(osg::Billboard::Mode::POINT_ROT_EYE);

    int i = 0;
    for (CountryNode::Map::const_iterator it = m->countryMap->cbegin(); it != m->countryMap->cend(); ++it)
    {
      osg::Vec3f pos = osgGaming::getCartesianFromPolar(it->second->getCenterLatLong());
      Country::Ptr country = m->countriesContainer->getCountry(it->first);

      if (!pos.valid() || !country)
        continue;

      std::string name = osgGaming::utf8ToLatin1(country->getName().c_str());
      /*std::string name = osgGaming::utf8ToLatin1(it->second->getCountryName().c_str()) + "\n";
      CountryData::Neighbors neighbors = it->second->getNeighborCountries();

      bool first = true;
      for (CountryData::Neighbors::iterator nit = neighbors.begin(); nit != neighbors.end(); ++nit)
      {
      name += (first ? "" : "; ") + osgGaming::utf8ToLatin1(nit->country->getCountryName().c_str());
      first = false;
      }*/

      osg::ref_ptr<osgText::Text> text = new osgText::Text();

      text->setCharacterSize(10);
      text->setFont(font);
      text->setText(name);
      text->setAlignment(osgText::Text::AlignmentType::CENTER_CENTER);
      text->setAxisAlignment(osgText::Text::AxisAlignment::XZ_PLANE);
      text->setDrawMode(osgText::Text::TEXT);
      text->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      //text->setDataVariance(osg::Object::DYNAMIC);

      //ref_ptr<ShapeDrawable> s = new ShapeDrawable(new Box());

      billboard->addDrawable(text);
      billboard->setPosition(i, pos * earthRadius * 106);

      i++;
    }

    m->switchNode->addChild(billboard);
  }

  void CountryNameOverlay::setEnabled(bool enabled)
  {
    if (m->enabled == enabled)
    {
      return;
    }

    m->enabled = enabled;

    m->switchNode->setNewChildDefaultValue(m->enabled);

    if (m->enabled)
    {
      m->switchNode->setAllChildrenOn();
    }
    else
    {
      m->switchNode->setAllChildrenOff();
    }
  }

}