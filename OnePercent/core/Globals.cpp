#include "core/Globals.h"

#include <assert.h>

namespace onep
{
  /*std::string branch_getStringFromType(int type)
  {
    switch (type)
    {
    case BRANCH_CONTROL:
      return "control";
    case BRANCH_BANKS:
      return "banks";
    case BRANCH_CONCERNS:
      return "concerns";
    case BRANCH_MEDIA:
      return "media";
    case BRANCH_POLITICS:
      return "politics";
    case BRANCH_UNDEFINED:
    default:
      assert(false);
      return "undefined";
    }
  }

  BranchType branch_getTypeFromString(std::string str)
  {
    if (str == "control")
      return BRANCH_CONTROL;
    if (str == "banks")
      return BRANCH_BANKS;
    if (str == "concerns")
      return BRANCH_CONCERNS;
    if (str == "media")
      return BRANCH_MEDIA;
    if (str == "politics")
      return BRANCH_POLITICS;

    assert(false);
    return BRANCH_UNDEFINED;
  }*/

  std::string countryValue_getStringFromType(int type)
  {
    switch (type)
    {
    case VALUE_ANGER:
      return "anger";
    case VALUE_BUYING_POWER:
      return "buyingpower";
    case VALUE_DEPT:
      return "dept";
    case VALUE_INTEREST:
      return "interest";
    case VALUE_PROPAGATED:
      return "propagated";
    case VALUE_PROPAGATION:
      return "propagation";
    case VALUE_UNDEFINED:
    default:
      assert(false);
      return "undefined";
    }
  }

  CountryValueType countryValue_getTypeFromString(std::string str)
  {
    if (str == "anger")
      return VALUE_ANGER;
    if (str == "buyingpower")
      return VALUE_BUYING_POWER;
    if (str == "dept")
      return VALUE_DEPT;
    if (str == "interest")
      return VALUE_INTEREST;
    if (str == "propagated")
      return VALUE_PROPAGATED;
    if (str == "propagation")
      return VALUE_PROPAGATION;

    assert(false);
    return VALUE_UNDEFINED;
  }

  std::string valueMethod_getStringFromType(int type)
  {
    switch (type)
    {
    case METHOD_ADD_CHANGE:
      return "add_change";
    case METHOD_ADD_INFLUENCE:
      return "add_influence";
    case METHOD_SET_BALANCE:
      return "set_balance";
    case METHOD_SET_VALUE:
      return "set_value";
    case METHOD_UNDEFINED:
    default:
      assert(false);
      return "undefined";
    }
  }

  ProgressingValueMethod valueMethod_getTypeFromString(std::string str)
  {
    if (str == "add_change")
      return METHOD_ADD_CHANGE;
    if (str == "add_influence")
      return METHOD_ADD_INFLUENCE;
    if (str == "set_balance")
      return METHOD_SET_BALANCE;
    if (str == "set_value")
      return METHOD_SET_VALUE;

    assert(false);
    return METHOD_UNDEFINED;
  }
}
