#include "CountryValues.h"

#include <osgGaming/Property.h>

namespace onep
{
  CountryValues::CountryValues(float wealth)
    : osg::Referenced()
    , m_container(new ProgressingValueContainer())
  {
    float startBuyingPower = osgGaming::PropertiesManager::getInstance()->getValue<float>(Param_MechanicsStartBuyingPowerName);

    createValue<float>(VALUE_ANGER, 0.0f, 1.0f, 0.0f);
    createValue<float>(VALUE_DEPT, 0.0f, wealth, 0.0f);
    createValue<float>(VALUE_INTEREST, 0.0f, 1.0f, 0.0f);
    createValue<float>(VALUE_BUYING_POWER, 0.0f, 1000.0f, 0.0f);

    createBranchValues<float>(VALUE_PROPAGATION, 0.0f, 1.0f, 0.0f);
    createBranchValues<float>(VALUE_PROPAGATED, 0.0f, 1.0f, 0.0f);

    getValue<float>(VALUE_INTEREST)->prepare(0.05f, METHOD_SET_VALUE);
    getValue<float>(VALUE_BUYING_POWER)->prepare(startBuyingPower, METHOD_SET_VALUE);
  }

  osg::ref_ptr<ProgressingValueContainer> CountryValues::getContainer()
  {
    return m_container;
  }
}