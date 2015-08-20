#include "CountryOverlay.h"

#include "Globals.h"

#include <osgGaming/Property.h>
#include <osgGaming/Helper.h>
#include <osg/Billboard>
#include <osgGaming/ResourceManager.h>

using namespace onep;

using namespace osg;

CountryOverlay::CountryOverlay()
	: PositionAttitudeTransform(),
	  _enabled(true)
{
	_switch = new Switch();
	_switch->setNewChildDefaultValue(_enabled);

	_switch->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	addChild(_switch);

	setScale(Vec3f(0.01, 0.01, 0.01));
}

bool CountryOverlay::getEnabled()
{
	return _enabled;
}

void CountryOverlay::setCountryMap(const Country::Map& countryMap)
{
	ref_ptr<osgText::Font> font = osgGaming::ResourceManager::getInstance()->loadFont("./GameData/fonts/coolvetica rg.ttf");

	float earthRadius = ~osgGaming::Property<float, Param_EarthRadiusName>();

	_countryMap = &countryMap;

	ref_ptr<Billboard> billboard = new Billboard();
	billboard->setMode(Billboard::Mode::POINT_ROT_EYE);

	int i = 0;
	for (Country::Map::const_iterator it = _countryMap->cbegin(); it != _countryMap->cend(); ++it)
	{
		Vec3f pos = osgGaming::getCartesianFromPolar(it->second->getCenterLatLong());
		if (!pos.valid())
		{
			continue;
		}

		std::string name = osgGaming::utf8ToLatin1(it->second->getCountryName().c_str());

		ref_ptr<osgText::Text> text = new osgText::Text();

		text->setCharacterSize(14);
		text->setFont(font);
		text->setText(name);
		text->setAlignment(osgText::Text::AlignmentType::CENTER_CENTER);
		text->setAxisAlignment(osgText::Text::AxisAlignment::XZ_PLANE);
		text->setDrawMode(osgText::Text::TEXT);
		text->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//text->setDataVariance(osg::Object::DYNAMIC);

		//ref_ptr<ShapeDrawable> s = new ShapeDrawable(new Box());

		billboard->addDrawable(text);
		billboard->setPosition(i, pos * earthRadius * 106);

		i++;
	}

	_switch->addChild(billboard);
}

void CountryOverlay::setEnabled(bool enabled)
{
	if (_enabled == enabled)
	{
		return;
	}

	_enabled = enabled;

	_switch->setNewChildDefaultValue(_enabled);

	if (_enabled)
	{
		_switch->setAllChildrenOn();
	}
	else
	{
		_switch->setAllChildrenOff();
	}
}