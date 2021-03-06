#include "CountryNameOverlay.h"

#include "simulation/ModelContainer.h"
#include "scripting/LuaCountriesTable.h"
#include "scripting/LuaCountry.h"
#include "scripting/LuaModel.h"

#include <osg/Billboard>
#include <osg/Switch>

#include <osgHelper/Helper.h>
#include <osgHelper/ResourceManager.h>

namespace onep
{
  struct CountryNameOverlay::Impl
  {
    Impl(osgHelper::ioc::Injector& injector)
      : configManager(injector.inject<LuaConfig>())
      , modelContainer(injector.inject<ModelContainer>())
      , enabled(true)
    {
      switchNode = new osg::Switch();
      switchNode->setNewChildDefaultValue(enabled);

      switchNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    }

    osg::ref_ptr<LuaConfig> configManager;

    ModelContainer::Ptr modelContainer;

    osg::ref_ptr<osg::Switch> switchNode;

    bool enabled;
  };

  CountryNameOverlay::CountryNameOverlay(osgHelper::ioc::Injector& injector)
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

  void CountryNameOverlay::initialize(const CountryPresenter::Map& countryPresenters)
  {
    auto font = osgHelper::ResourceManager::getDefaultFont();

    float earthRadius = m->configManager->getNumber<float>("earth.radius");

    osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard();
    billboard->setMode(osg::Billboard::Mode::POINT_ROT_EYE);

    int i = 0;
    for (const auto& presenter : countryPresenters)
    {
      osg::Vec3f pos = osgHelper::getCartesianFromPolar(presenter.second->getCenterLatLong());
      LuaCountry::Ptr country = m->modelContainer->getModel()->getCountriesTable()->getCountryById(presenter.first);

      if (!pos.valid() || !country)
        continue;

      std::string name = osgHelper::utf8ToLatin1(country->getName().c_str());
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

      if (font)
      {
        text->setFont(font);
      }

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