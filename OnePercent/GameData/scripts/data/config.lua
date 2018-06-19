core.config.extend(
{
  camera = {
    min_distance = 7.5,
    max_distance = 25.0,
    max_latitude = 1.414, -- PI / 2.0 * 0.9
    zoom_speed = 0.85,
    zoom_speed_factor = 3.0,
    scroll_speed = 0.001,
    rotation_speed = 0.003,
    country_zoom = 1.5,
  },
  earth = {
    radius = 6.371,
    sphere_stacks = 96,
    sphere_slices = 192,
    clouds_height = 0.004,
    clouds_speed = 0.3,
    clouds_morph_speed = 20.0,
    atmosphere_height = 0.06,
    atmosphere_color = { 0.1981, 0.4656, 0.8625, 0.75 },
    scattering_depth = 0.25,
    scattering_intensity = 0.8
  },
  sun = {
    distance = 149600.0,
    radius_pm2 = 0.00000206552 -- pow(695.8f, -2.0f)
  },
  mechanics = {
    days_in_year = 356.0
  }
})
