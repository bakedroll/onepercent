#pragma once

#include <string>

namespace onep
{
	char Param_CameraMinDistanceName[];
	char Param_CameraMaxDistanceName[];
	char Param_CameraMaxLatitudeName[];
	char Param_CameraZoomSpeedName[];
	char Param_CameraZoomSpeedFactorName[];
	char Param_CameraScrollSpeedName[];
	char Param_CameraRotationSpeedName[];
	char Param_CameraCountryZoomName[];

	char Param_EarthRadiusName[];
	char Param_EarthSphereStacksName[];
	char Param_EarthSphereSlicesName[];

	char Param_EarthCloudsHeightName[];
	char Param_EarthCloudsSpeedName[];
	char Param_EarthCloudsMorphSpeedName[];

	char Param_EarthAtmosphereHeightName[];
	char Param_EarthAtmosphereColorName[];

	char Param_EarthScatteringDepthName[];
	char Param_EarthScatteringIntensityName[];

	char Param_SunDistanceName[];
	char Param_SunRadiusPm2Name[];

	char Param_MechanicsDaysInYearName[];
	char Param_MechanicsStartBuyingPowerName[];

	char Param_LocalizationInfoTextDay[];
	char Param_LocalizationInfoTextLoading[];

  const int NUM_SKILLBRANCHES = 5;

  typedef enum _branchType
  {
    BRANCH_BANKS = 0,
    BRANCH_CONTROL = 1,
    BRANCH_MEDIA = 2,
    BRANCH_CONCERNS = 3,
    BRANCH_POLITICS = 4,
    BRANCH_UNDEFINED = -1
  } BranchType;

  typedef enum _countryValueType
  {
    VALUE_ANGER,
    VALUE_DEPT,
    VALUE_INTEREST,
    VALUE_BUYING_POWER,
    VALUE_PROPAGATION,
    VALUE_PROPAGATED,
    VALUE_UNDEFINED = -1
  } CountryValueType;

  typedef enum _progressingValueMethod
  {
    METHOD_SET_BALANCE,
    METHOD_SET_VALUE,
    METHOD_ADD_INFLUENCE,
    METHOD_ADD_CHANGE,
    METHOD_UNDEFINED = -1
  } ProgressingValueMethod;

  std::string branch_getStringFromType(int type);
  BranchType branch_getTypeFromString(std::string str);

  std::string countryValue_getStringFromType(int type);
  CountryValueType countryValue_getTypeFromString(std::string str);

  std::string valueMethod_getStringFromType(int type);
  ProgressingValueMethod valueMethod_getTypeFromString(std::string str);
}