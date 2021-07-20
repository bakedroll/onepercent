cmake_minimum_required(VERSION 3.4)

set(CMAKE_SCRIPTS_DIRECTORY "" CACHE STRING "The CMake scripts directory")
if(EXISTS ${CMAKE_SCRIPTS_DIRECTORY}/scripts.cmake)
  include(${CMAKE_SCRIPTS_DIRECTORY}/scripts.cmake)
else()
  message(FATAL_ERROR "Please specify CMAKE_SCRIPTS_DIRECTORY")
  return()
endif()

project(OnePercent)

option(LUADOC_ENABLE "enable luadoc" OFF)

add_subdirectory(VectorizerLib)
add_subdirectory(VectorizerLibTest)
add_subdirectory(Vectorizer)

add_subdirectory(OnePercent)

add_project(QtOsgBridgeApplication PATH QtOsgBridge)
add_project(osgHelperTest PATH osgHelper)
add_project(luaHelperTest PATH luaHelper)

make_projects()

if (MSVC)
  osg_copy_additional_binaries(zlib libpng16)
  osg_copy_plugins(osgdb_3ds osgdb_freetype osgdb_obj osgdb_png)

  qt_copy_plugins(platforms/qwindows styles/qwindowsvistastyle)
endif()

# configure scripts
set(TEMPLATE_PATH_RESOURCES "${CMAKE_SOURCE_DIR}/Resources")
set(TEMPLATE_PATH_ONEPERCENT_SRC "${CMAKE_SOURCE_DIR}/OnePercent")
set(TEMPLATE_PATH_ONEPERCENT_DST "${CMAKE_BINARY_DIR}")
set(TEMPLATE_PATH_GAMEDATA_SRC "${TEMPLATE_PATH_ONEPERCENT_SRC}/GameData")
set(TEMPLATE_PATH_GAMEDATA_DST "${TEMPLATE_PATH_ONEPERCENT_DST}/GameData")

configure_file("CMakeTemplates/generate_boundaries_and_countries.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_boundaries_and_countries.py")
configure_file("CMakeTemplates/generate_boundaries_and_countries_no_distance.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_boundaries_and_countries_no_distance.py")
configure_file("CMakeTemplates/generate_colormap.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_colormap.py")
configure_file("CMakeTemplates/generate_merged_textures.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_merged_textures.py")

set(COPY_DIR_SOURCE "${TEMPLATE_PATH_GAMEDATA_SRC}/scripts")
set(COPY_DIR_TARGET "${TEMPLATE_PATH_GAMEDATA_DST}/scripts")
configure_file("CMakeTemplates/copydir.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/updateScripts.py)

set(COPY_DIR_SOURCE "${TEMPLATE_PATH_ONEPERCENT_SRC}/Mods")
set(COPY_DIR_TARGET "${TEMPLATE_PATH_ONEPERCENT_DST}/Mods")
configure_file("CMakeTemplates/copydir.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/updateMods.py)

set(COPY_DIR_SOURCE "${TEMPLATE_PATH_GAMEDATA_SRC}/shaders")
set(COPY_DIR_TARGET "${TEMPLATE_PATH_GAMEDATA_DST}/shaders")
configure_file("CMakeTemplates/copydir.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/updateShaders.py)

configure_file("CMakeTemplates/loc.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/loc.py)
