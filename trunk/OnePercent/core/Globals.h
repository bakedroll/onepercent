#pragma once

#include <string>

namespace onep
{
  static const std::string Param_CameraMinDistanceName = "camera/min_distance";
  static const std::string Param_CameraMaxDistanceName = "camera/max_distance";
  static const std::string Param_CameraMaxLatitudeName = "camera/max_latitude";
  static const std::string Param_CameraZoomSpeedName = "camera/zoom_speed";
  static const std::string Param_CameraZoomSpeedFactorName = "camera/zoom_speed_factor";
  static const std::string Param_CameraScrollSpeedName = "camera/scroll_speed";
  static const std::string Param_CameraRotationSpeedName = "camera/rotation_speed";
  static const std::string Param_CameraCountryZoomName = "camera/country_zoom";

  static const std::string Param_EarthRadiusName = "earth/radius";
  static const std::string Param_EarthSphereStacksName = "earth/sphere_stacks";
  static const std::string Param_EarthSphereSlicesName = "earth/sphere_slices";

  static const std::string Param_EarthCloudsHeightName = "earth/clouds_height";
  static const std::string Param_EarthCloudsSpeedName = "earth/clouds_speed";
  static const std::string Param_EarthCloudsMorphSpeedName = "earth/clouds_morph_speed";

  static const std::string Param_EarthAtmosphereHeightName = "earth/atmosphere_height";
  static const std::string Param_EarthAtmosphereColorName = "earth/atmosphere_color";

  static const std::string Param_EarthScatteringDepthName = "earth/scattering_depth";
  static const std::string Param_EarthScatteringIntensityName = "earth/scattering_intensity";

  static const std::string Param_SunDistanceName = "sun/distance";
  static const std::string Param_SunRadiusPm2Name = "sun/radius_pm2";

  static const std::string Param_MechanicsDaysInYearName = "mechanics/days_in_year";
  static const std::string Param_MechanicsStartBuyingPowerName = "mechanics/start_buying_power";

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

  std::string countryValue_getStringFromType(int type);
  CountryValueType countryValue_getTypeFromString(std::string str);

  std::string valueMethod_getStringFromType(int type);
  ProgressingValueMethod valueMethod_getTypeFromString(std::string str);
}
