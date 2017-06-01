cmake_minimum_required (VERSION 2.8.11)

include("CMake/scripts.cmake")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake )
set(ERROR_MESSAGES "")

set(PROJECT_BIN_DIR "bin")
set(PROJECT_LIB_DIR "lib")

project (OnePercent)

# update output directories, see http://stackoverflow.com/questions/7229571/cmake-visual-studio-debug-folder
# First for the generic no-config case (e.g. with mingw)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_LIB_DIR})
# Second, for multi-config builds (e.g. msvc)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${PROJECT_LIB_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${PROJECT_LIB_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# find Qt5
set(QT5_ROOT_DIRECTORY "NOTFOUND" CACHE STRING "Qt5 root directory")
if (EXISTS ${QT5_ROOT_DIRECTORY})
  adjust_cached_path(QT5_ROOT_DIRECTORY "Qt5 root directory" ${QT5_ROOT_DIRECTORY})
endif()

set(CMAKE_PREFIX_PATH ${QT5_ROOT_DIRECTORY})

set(QT5_MODULES
  Core
  Gui
  Widgets
  OpenGL)
  
foreach(module ${QT5_MODULES})
  find_package(Qt5${module})
endforeach()

#find OpenSceneGraph
find_package(OpenSceneGraph COMPONENTS
  osg osgViewer osgUtil osgGA osgDB osgText OpenThreads)

# pthread
find_package(Threads)

# optional for Vectorizer
find_package(OpenCV)

# optional for Tests
set(GTEST_ROOT_DIRECTORY "NOTFOUND" CACHE STRING "gtest root directory")
if (EXISTS ${GTEST_ROOT_DIRECTORY})
  adjust_cached_path(GTEST_ROOT_DIRECTORY "gtest root directory" ${GTEST_ROOT_DIRECTORY})
endif()

set(GTEST_ROOT ${QT5_ROOT_DIRECTORY})

find_package(GTest)

# osgPPU
set(OSGPPU_INCLUDE_DIR "NOTFOUND" CACHE STRING "osgPPU include directory")
if (EXISTS ${OSGPPU_INCLUDE_DIR})
  adjust_cached_path(OSGPPU_INCLUDE_DIR "osgPPU include directory" ${OSGPPU_INCLUDE_DIR})
else()
  set(ERROR_MESSAGES ${ERROR_MESSAGES} "Please specify OSGPPU_INCLUDE_DIR\n")
endif()

set(OSGPPU_LIBRARY_RELEASE "NOTFOUND" CACHE STRING "osgPPU library")
if (EXISTS ${OSGPPU_LIBRARY_RELEASE})
  adjust_cached_path(OSGPPU_LIBRARY_RELEASE "osgPPU library" ${OSGPPU_LIBRARY_RELEASE})
else()
  set(ERROR_MESSAGES ${ERROR_MESSAGES} "Please specify OSGPPU_LIBRARY_RELEASE\n")
endif()

set(OSGPPU_LIBRARY_DEBUG "NOTFOUND" CACHE STRING "osgPPU debug library")
if (EXISTS ${OSGPPU_LIBRARY_DEBUG})
  adjust_cached_path(OSGPPU_LIBRARY_DEBUG "osgPPU debug library" ${OSGPPU_LIBRARY_DEBUG})
else()
  set(OSGPPU_LIBRARY_DEBUG ${OSGPPU_LIBRARY_RELEASE})
endif()

# Check for existence of required directories
if ("${Qt5Core_LIBRARIES}" STREQUAL "")
  set(ERROR_MESSAGES ${ERROR_MESSAGES} "Please specify QT5_ROOT_DIRECTORY\n")
endif()

if (NOT EXISTS ${OSG_INCLUDE_DIR})
  set(ERROR_MESSAGES ${ERROR_MESSAGES} "Please specify OSG_INCLUDE_DIR\n")
else()
  adjust_cached_path(OSG_INCLUDE_DIR "Path to a file." ${OSG_INCLUDE_DIR})
endif()

if (NOT EXISTS ${OSG_LIBRARY})
  set(ERROR_MESSAGES ${ERROR_MESSAGES} "Please specify OSG_LIBRARY\n")
else()
  adjust_cached_path(OSG_LIBRARY "Path to a library." ${OSG_LIBRARY})
endif()

if (NOT EXISTS ${RAPIDXML_INCLUDE_DIR})
  set(ERROR_MESSAGES ${ERROR_MESSAGES} "Please specify RAPIDXML_INCLUDE_DIR\n")
else()
  adjust_cached_path(RAPIDXML_INCLUDE_DIR "Path to a file." ${RAPIDXML_INCLUDE_DIR})
endif()

# check for debug libraries
if (NOT EXISTS ${OSG_LIBRARY_DEBUG})
  set(OSG_LIBRARY_DEBUG ${OSG_LIBRARY})
endif()

# print errors
if (NOT "${ERROR_MESSAGES}" STREQUAL "")
  message(FATAL_ERROR ${ERROR_MESSAGES})
endif()

adjust_cached_path(OSG_LIBRARY_DEBUG "Path to a library." ${OSG_LIBRARY_DEBUG})

option(OSG_LIBRARIES_DEBUG_SUFFIX "osg debug libraries suffix" ON)

if (OSG_LIBRARIES_DEBUG_SUFFIX)
  set(OSG_LIBRARY_SUFFIX "d")
else()
  set(OSG_LIBRARY_SUFFIX "\"\"")
endif()

# find osg binary directory
if (WIN32)
  set(OSG_BINARY "NOTFOUND" CACHE STRING "OSG binary directory")
  set(OSG_BINARY_DEBUG "NOTFOUND" CACHE STRING "OSG debug binary directory")

  if (NOT EXISTS ${OSG_BINARY})
    if (EXISTS ${OSG_LIBRARY}/../bin)
      set(OSG_BINARY_TMP ${OSG_LIBRARY}/../bin)
      
      adjust_cached_path(OSG_BINARY "OSG binary directory" ${OSG_BINARY_TMP})
    else()
      message(WARNING "OSG binary directory not found")
    endif()
  endif()

  adjust_cached_path(OSG_BINARY "OSG binary directory" ${OSG_BINARY})
  if (NOT EXISTS ${OSG_BINARY_DEBUG} AND EXISTS ${OSG_BINARY})
    adjust_cached_path(OSG_BINARY_DEBUG "OSG debug binary directory" ${OSG_BINARY})
  endif()

  # find osg plugins directory
  set(OSG_PLUGINS "NOTFOUND" CACHE STRING "OSG plugins directory")
  set(OSG_PLUGINS_DEBUG "NOTFOUND" CACHE STRING "OSG plugins directory")

  if (EXISTS ${OSG_BINARY} AND NOT EXISTS ${OSG_PLUGINS})  
    if (EXISTS ${OSG_BINARY}/osgPlugins)
      adjust_cached_path(OSG_PLUGINS "OSG plugins directory" ${OSG_BINARY}/osgPlugins-${OPENSCENEGRAPH_VERSION})
    else()
      message(WARNING "OSG plugins directory not found")
    endif()
  endif()

  adjust_cached_path(OSG_PLUGINS "OSG plugins directory" ${OSG_PLUGINS})
  if (NOT EXISTS ${OSG_PLUGINS_DEBUG} AND EXISTS ${OSG_PLUGINS})
    adjust_cached_path(OSG_PLUGINS_DEBUG "OSG debug binary directory" ${OSG_PLUGINS})
  endif()
endif()

# find OpenCV directories
adjust_cached_path_if_exists(OPENCV_INCLUDE_DIR "OpenCV include directory" ${OPENCV_INCLUDE_DIR})
adjust_cached_path_if_exists(OPENCV_LIB_DIR_OPT "OpenCV release lib directory" ${OPENCV_LIB_DIR_OPT})
adjust_cached_path_if_exists(OPENCV_LIB_DIR_DBG "OpenCV debug lib directory" ${OPENCV_LIB_DIR_DBG})
adjust_cached_path_if_exists(OPENCV_BIN_DIR_OPT "OpenCV release bin directory" ${OPENCV_BIN_DIR_OPT})
adjust_cached_path_if_exists(OPENCV_BIN_DIR_DBG "OpenCV debug bin directory" ${OPENCV_BIN_DIR_DBG})

if (EXISTS ${OPENCV_LIB_DIR_OPT} AND NOT EXISTS ${OPENCV_LIB_DIR_DBG})
  adjust_cached_path(OPENCV_LIB_DIR_DBG "OpenCV debug lib directory" ${OPENCV_LIB_DIR_OPT})
endif()

if (EXISTS ${OPENCV_BIN_DIR_OPT} AND NOT EXISTS ${OPENCV_BIN_DIR_DBG})
  adjust_cached_path(OPENCV_BIN_DIR_DBG "OpenCV debug bin directory" ${OPENCV_BIN_DIR_OPT})
endif()

if (EXISTS ${OPENCV_LIB_DIR_OPT} AND EXISTS ${OPENCV_INCLUDE_DIR})
  set(OPENCV_FOUND true)
  
  option(OPENCV_LIBRARIES_DEBUG_SUFFIX "OpenCV debug libraries suffix" ON)

  if (OPENCV_LIBRARIES_DEBUG_SUFFIX)
    set(OPENCV_LIBRARY_SUFFIX "d")
  else()
    set(OPENCV_LIBRARY_SUFFIX "\"\"")
  endif()
endif()

# add subdirectories
add_subdirectory(osgGaming)
add_subdirectory(OnePercent)

if (OPENCV_FOUND)
  add_subdirectory(Vectorizer)
else()
  message(WARNING "OpenCV not found: ignoring project Vectorizer")
endif()

if (GTEST_FOUND)
  add_subdirectory(osgGamingTest)
else()
  message(WARNING "GTest not found: ignoring project osgGamingTest")
endif()

# copy shared dlls
if (WIN32 AND EXISTS ${OSG_BINARY})
  copy_dlls(${OSG_BINARY} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "osg100-" ""
    "osg osgDB osgGA osgText osgUtil osgViewer")
  copy_dlls(${OSG_BINARY} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" ""
    "osgPPU")
  copy_dlls(${OSG_BINARY} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "ot20-" ""
    "OpenThreads")
  
  if (EXISTS ${OSG_PLUGINS})
    copy_dlls(${OSG_PLUGINS} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "osgdb_" ""
      "freetype glsl jpeg png")
  endif()
  
  if (OSG_LIBRARIES_DEBUG_SUFFIX AND EXISTS ${OSG_BINARY_DEBUG})
    copy_dlls(${OSG_BINARY_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "osg100-" ${OSG_LIBRARY_SUFFIX}
      "osg osgDB osgGA osgText osgUtil osgViewer")
    copy_dlls(${OSG_BINARY_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" ${OSG_LIBRARY_SUFFIX}
      "osgPPU")
    copy_dlls(${OSG_BINARY_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "ot20-" ${OSG_LIBRARY_SUFFIX}
      "OpenThreads")
    
    if (EXISTS ${OSG_PLUGINS_DEBUG})
      copy_dlls(${OSG_PLUGINS_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "osgdb_" ${OSG_LIBRARY_SUFFIX}
        "freetype glsl jpeg png")
    endif()
  endif()
endif()

if (WIN32)
  foreach(module ${QT5_MODULES})
    get_target_property(MODULE_LOCATION Qt5::${module} LOCATION)
    get_filename_component(MODULE_DIR ${MODULE_LOCATION} DIRECTORY)
    get_filename_component(MODULE_FILENAME ${MODULE_LOCATION} NAME)
    get_filename_component(MODULE_EXT ${MODULE_LOCATION} EXT)
    get_filename_component(MODULE_NAME_WE ${MODULE_LOCATION} NAME_WE)
    
    message(STATUS "copy library ${MODULE_FILENAME}")
    configure_file(${MODULE_LOCATION} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/${MODULE_FILENAME} COPYONLY)
    
    set(MODULE_LOCATION_DBG ${MODULE_DIR}/${MODULE_NAME_WE}d${MODULE_EXT})
    get_filename_component(MODULE_FILENAME_DBG ${MODULE_LOCATION_DBG} NAME)
    
    message(STATUS "copy library ${MODULE_FILENAME_DBG}")
    configure_file(${MODULE_LOCATION_DBG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/${MODULE_FILENAME_DBG} COPYONLY)
  endforeach()
endif()


# configure scripts
set(TEMPLATE_PATH_RESOURCES "${CMAKE_SOURCE_DIR}/Resources")
set(TEMPLATE_PATH_GAMEDATA_SRC "${CMAKE_SOURCE_DIR}/OnePercent/GameData")
set(TEMPLATE_PATH_GAMEDATA_DST "${CMAKE_BINARY_DIR}/OnePercent/GameData")

configure_file("CMake/templates/generate_boundaries_and_countries.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_boundaries_and_countries.py")
configure_file("CMake/templates/generate_colormap.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_colormap.py")
configure_file("CMake/templates/generate_merged_textures.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_merged_textures.py")