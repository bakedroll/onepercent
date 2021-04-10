cmake_minimum_required (VERSION 2.8.11)

include("CMake/scripts.cmake")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)
set(ERROR_MESSAGES "")

set(PROJECT_BIN_DIR "bin")
set(PROJECT_LIB_DIR "lib")

project (OnePercent)

if (MSVC)
# generate pdb files in release build
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi" CACHE STRING "" FORCE)
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF" CACHE STRING "" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF" CACHE STRING "" FORCE)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_STANDARD 17)
endif()

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

# Compiler flag for Qt that was built with -reduce-relocations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

option(BUILD_ENABLE_CONSOLE "Builds a console application (Windows only)" ON)
option(OSG_LIBRARIES_DEBUG_SUFFIX "osg debug libraries suffix" ON)
option(LUADOC_ENABLE "enable luadoc" OFF)

# find Qt
set(QT_VERSION "6" CACHE STRING "Qt version")

set(QT_ROOT_DIRECTORY "NOTFOUND" CACHE STRING "Qt root directory")
if (EXISTS ${QT_ROOT_DIRECTORY})
  adjust_cached_path(QT_ROOT_DIRECTORY "Qt root directory" ${QT_ROOT_DIRECTORY})
  set(CMAKE_PREFIX_PATH ${QT_ROOT_DIRECTORY})
endif()

set(QT_MODULES
  Core
  Gui
  Widgets
  OpenGL)

if ("${QT_VERSION}" STREQUAL "6")
  set(QT_MODULES
    ${QT_MODULES}
    OpenGLWidgets)
endif()

foreach(module ${QT_MODULES})
  find_package(Qt${QT_VERSION}${module} REQUIRED)
endforeach()

if ("${Qt${QT_VERSION}Core_LIBRARIES}" STREQUAL "")
  set(ERROR_MESSAGES ${ERROR_MESSAGES} "Please specify QT_ROOT_DIRECTORY\n")
endif()

#find OpenSceneGraph
find_package(OpenSceneGraph COMPONENTS
  osg osgViewer osgUtil osgGA osgDB osgText OpenThreads)

adjust_cached_path_if_exists(OSG_INCLUDE_DIR "Path to a file." "${OSG_INCLUDE_DIR}" MESSAGES ERROR_MESSAGES)
adjust_cached_path_if_exists(OSG_LIBRARY "Path to a library." "${OSG_LIBRARY}" MESSAGES ERROR_MESSAGES)

SET(OSG_VERSION_PREFIX "" CACHE STRING "The osg version prefix")
SET(OPENTHREADS_VERSION_PREFIX "" CACHE STRING "The OpenThreads version prefix")

if (NOT EXISTS ${OSG_LIBRARY_DEBUG})
  set(OSG_LIBRARY_DEBUG ${OSG_LIBRARY})
endif()

adjust_cached_path(OSG_LIBRARY_DEBUG "Path to a library." ${OSG_LIBRARY_DEBUG})

if (OSG_LIBRARIES_DEBUG_SUFFIX)
  set(OSG_LIBRARY_SUFFIX "d")
else()
  set(OSG_LIBRARY_SUFFIX "\"\"")
endif()

# osgPPU
adjust_cached_path_if_exists(OSGPPU_INCLUDE_DIR "osgPPU include directory" "${OSGPPU_INCLUDE_DIR}" MESSAGES ERROR_MESSAGES CACHE)
adjust_cached_path_if_exists(OSGPPU_LIBRARY_RELEASE "osgPPU release library" "${OSGPPU_LIBRARY_RELEASE}" MESSAGES ERROR_MESSAGES CACHE)
adjust_cached_path_if_exists(OSGPPU_LIBRARY_DEBUG "osgPPU debug library" "${OSGPPU_LIBRARY_DEBUG}" MESSAGES ERROR_MESSAGES DEFAULT ${OSGPPU_LIBRARY_RELEASE} CACHE)

set(OSGPPU_LIBRARY_DEBUG "NOTFOUND" CACHE STRING "osgPPU debug library")
if (EXISTS ${OSGPPU_LIBRARY_DEBUG})
  adjust_cached_path(OSGPPU_LIBRARY_DEBUG "osgPPU debug library" ${OSGPPU_LIBRARY_DEBUG})
else()
  set(OSGPPU_LIBRARY_DEBUG ${OSGPPU_LIBRARY_RELEASE})
endif()

# optional for Vectorizer
find_package(OpenCV)

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
  
  SET(OPENCV_VERSION_SUFFIX "300" CACHE STRING "The OpenCV version suffix")
  option(OPENCV_LIBRARIES_DEBUG_SUFFIX "OpenCV debug libraries suffix" ON)

  if (OPENCV_LIBRARIES_DEBUG_SUFFIX)
    set(OPENCV_LIBRARY_SUFFIX "d")
  else()
    set(OPENCV_LIBRARY_SUFFIX "\"\"")
  endif()
endif()
  
# pthread
find_package(Threads)

# Lua
if (NOT EXISTS ${LUA_INCLUDE_PREFIX})
  if (NOT EXISTS ${LUA_INCLUDE_DIR})
    find_package(Lua REQUIRED)
  endif()
endif()

if (NOT EXISTS ${LUA_LIB_DEBUG})
  set(LUA_LIB_DEBUG ${LUA_LIB_RELEASE} CACHE STRING "Lua debug library")
endif()

#adjust_cached_path_if_exists(LUA_INCLUDE_PREFIX "Lua include dir" "${LUA_INCLUDE_PREFIX}" MESSAGES ERROR_MESSAGES CACHE)
adjust_cached_path_if_exists(LUA_LIB_RELEASE "Lua release library" "${LUA_LIB_RELEASE}" MESSAGES ERROR_MESSAGES)
adjust_cached_path_if_exists(LUA_LIB_DEBUG "Lua debug library" "${LUA_LIB_DEBUG}" MESSAGES ERROR_MESSAGES CACHE)

# LuaBridge
adjust_cached_path_if_exists(LUABRIDGE_INCLUDE_DIR "LuaBridge include dir" "${LUABRIDGE_INCLUDE_DIR}" MESSAGES ERROR_MESSAGES CACHE)

# optional for Tests
set(GTEST_ROOT_DIRECTORY "NOTFOUND" CACHE STRING "gtest root directory")
if (EXISTS ${GTEST_ROOT_DIRECTORY})
  adjust_cached_path(GTEST_ROOT_DIRECTORY "gtest root directory" ${GTEST_ROOT_DIRECTORY})
endif()

set(GTEST_ROOT ${QT_ROOT_DIRECTORY})

find_package(GTest)
find_package(OpenGL)

# print errors
if (NOT "${ERROR_MESSAGES}" STREQUAL "")
  message(FATAL_ERROR ${ERROR_MESSAGES})
endif()

# find osg binary directory
if (WIN32)
  set(OSG_BINARY "NOTFOUND" CACHE STRING "OSG binary directory")
  set(OSG_BINARY_DEBUG "NOTFOUND" CACHE STRING "OSG debug binary directory")
  set(OSGPPU_BINARY "NOTFOUND" CACHE STRING "OSGPPU binary directory")
  set(OSGPPU_BINARY_DEBUG "NOTFOUND" CACHE STRING "OSGPPU debug binary directory")
  
  if (NOT EXISTS ${OSG_BINARY})
    if (EXISTS ${OSG_LIBRARY}/../bin)
      set(OSG_BINARY_TMP ${OSG_LIBRARY}/../bin)
      
      adjust_cached_path(OSG_BINARY "OSG binary directory" ${OSG_BINARY_TMP})
    else()
      message(WARNING "OSG binary directory not found")
    endif()
  endif()
  
  if (NOT EXISTS ${OSGPPU_BINARY})
	  get_filename_component(OSGPPU_LIBRARY_DIRECTORY ${OSGPPU_LIBRARY_RELEASE} DIRECTORY)
    if (EXISTS ${OSGPPU_LIBRARY_DIRECTORY}/Release)
      set(OSGPPU_BINARY_TMP ${OSGPPU_LIBRARY_DIRECTORY}/Release)
      
      adjust_cached_path(OSGPPU_BINARY "OSGPPU binary directory" ${OSGPPU_BINARY_TMP})
    else()
      message(WARNING "OSGPPU binary directory not found")
    endif()
  endif()

  adjust_cached_path(OSG_BINARY "OSG binary directory" ${OSG_BINARY})
  if (NOT EXISTS ${OSG_BINARY_DEBUG} AND EXISTS ${OSG_BINARY})
    adjust_cached_path(OSG_BINARY_DEBUG "OSG debug binary directory" ${OSG_BINARY})
  endif()
  
  adjust_cached_path(OSGPPU_BINARY "OSGPPU binary directory" ${OSGPPU_BINARY})
  if (NOT EXISTS ${OSGPPU_BINARY_DEBUG} AND EXISTS ${OSGPPU_BINARY})
    adjust_cached_path(OSGPPU_BINARY_DEBUG "OSGPPU debug binary directory" ${OSGPPU_BINARY}/../Debug)
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

# add subdirectories
add_subdirectory(osgHelper)
add_subdirectory(QtOsgBridge)
add_subdirectory(QtOsgBridgeApplication)

if (LUADOC_ENABLE)
  add_subdirectory(luadoc)

  if (GTEST_FOUND)
    add_subdirectory(luadocTest)
  endif()
endif()

add_subdirectory(OnePercent)

if (OPENCV_FOUND)
  add_subdirectory(VectorizerLib)
  add_subdirectory(Vectorizer)
else()
  message(WARNING "OpenCV not found: ignoring project Vectorizer")
endif()

if (GTEST_FOUND)
  add_subdirectory(osgHelperTest)
  
  if (OPENCV_FOUND)
    add_subdirectory(VectorizerLibTest)
  endif()
else()
  message(WARNING "GTest not found: ignoring project osgHelperTest")
endif()

# copy shared dlls
if (WIN32)
  if (EXISTS ${OSG_BINARY})
    copy_dlls(${OSG_BINARY} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "${OSG_VERSION_PREFIX}" ""
      "osg osgDB osgGA osgText osgUtil osgViewer")
    copy_dlls(${OSG_BINARY} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "${OPENTHREADS_VERSION_PREFIX}" ""
      "OpenThreads")
    
    if (EXISTS ${OSG_PLUGINS})
      copy_dlls(${OSG_PLUGINS} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "osgdb_" ""
        "freetype png 3ds obj")
    endif()
    
    if (OSG_LIBRARIES_DEBUG_SUFFIX AND EXISTS ${OSG_BINARY_DEBUG})
      copy_dlls(${OSG_BINARY_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "${OSG_VERSION_PREFIX}" ${OSG_LIBRARY_SUFFIX}
        "osg osgDB osgGA osgText osgUtil osgViewer")
      copy_dlls(${OSG_BINARY_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "${OPENTHREADS_VERSION_PREFIX}" ${OSG_LIBRARY_SUFFIX}
        "OpenThreads")
      
      if (EXISTS ${OSG_PLUGINS_DEBUG})
        copy_dlls(${OSG_PLUGINS_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "osgdb_" ${OSG_LIBRARY_SUFFIX}
          "freetype png 3ds obj")
      endif()
    endif()
  endif()
  
  if (EXISTS ${OSGPPU_BINARY})
    copy_dlls(${OSGPPU_BINARY} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" "" "osgPPU")
  endif()
  
  if (EXISTS ${OSGPPU_BINARY_DEBUG})
    copy_dlls(${OSGPPU_BINARY_DEBUG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" "d" "osgPPU")
  endif()
  
  if (GTEST_FOUND)
    get_filename_component(GTEST_LIBRARY_DLL_PATH ${GTEST_LIBRARY} DIRECTORY)
    get_filename_component(GTEST_LIBRARY_DLL_FILEWE ${GTEST_LIBRARY} NAME_WE)
    
    get_filename_component(GTEST_LIBRARY_DEBUG_DLL_PATH ${GTEST_LIBRARY_DEBUG} DIRECTORY)
    get_filename_component(GTEST_LIBRARY_DEBUG_DLL_FILEWE ${GTEST_LIBRARY_DEBUG} NAME_WE)
    
    get_filename_component(GTEST_MAIN_LIBRARY_DLL_PATH ${GTEST_MAIN_LIBRARY} DIRECTORY)
    get_filename_component(GTEST_MAIN_LIBRARY_DLL_FILEWE ${GTEST_MAIN_LIBRARY} NAME_WE)
    
    get_filename_component(GTEST_MAIN_LIBRARY_DEBUG_DLL_PATH ${GTEST_MAIN_LIBRARY_DEBUG} DIRECTORY)
    get_filename_component(GTEST_MAIN_LIBRARY_DEBUG_DLL_FILEWE ${GTEST_MAIN_LIBRARY_DEBUG} NAME_WE)
    
    copy_dlls(${GTEST_LIBRARY_DLL_PATH} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" "" ${GTEST_LIBRARY_DLL_FILEWE})
    copy_dlls(${GTEST_LIBRARY_DEBUG_DLL_PATH} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" "" ${GTEST_LIBRARY_DEBUG_DLL_FILEWE})
    copy_dlls(${GTEST_MAIN_LIBRARY_DLL_PATH} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" "" ${GTEST_MAIN_LIBRARY_DLL_FILEWE})
    copy_dlls(${GTEST_MAIN_LIBRARY_DEBUG_DLL_PATH} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "" "" ${GTEST_MAIN_LIBRARY_DEBUG_DLL_FILEWE})
  endif()
endif()

if (WIN32)
  foreach(module ${QT_MODULES})
    get_target_property(MODULE_LOCATION Qt${QT_VERSION}::${module} LOCATION)
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
set(TEMPLATE_PATH_ONEPERCENT_SRC "${CMAKE_SOURCE_DIR}/OnePercent")
set(TEMPLATE_PATH_ONEPERCENT_DST "${CMAKE_BINARY_DIR}/OnePercent")
set(TEMPLATE_PATH_GAMEDATA_SRC "${TEMPLATE_PATH_ONEPERCENT_SRC}/GameData")
set(TEMPLATE_PATH_GAMEDATA_DST "${TEMPLATE_PATH_ONEPERCENT_DST}/GameData")

configure_file("CMake/templates/generate_boundaries_and_countries.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_boundaries_and_countries.py")
configure_file("CMake/templates/generate_boundaries_and_countries_no_distance.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_boundaries_and_countries_no_distance.py")
configure_file("CMake/templates/generate_colormap.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_colormap.py")
configure_file("CMake/templates/generate_merged_textures.py.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/generate_merged_textures.py")

set(COPY_DIR_SOURCE "${TEMPLATE_PATH_GAMEDATA_SRC}/scripts")
set(COPY_DIR_TARGET "${TEMPLATE_PATH_GAMEDATA_DST}/scripts")
configure_file("CMake/templates/copydir.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/updateScripts.py)

set(COPY_DIR_SOURCE "${TEMPLATE_PATH_ONEPERCENT_SRC}/Mods")
set(COPY_DIR_TARGET "${TEMPLATE_PATH_ONEPERCENT_DST}/Mods")
configure_file("CMake/templates/copydir.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/updateMods.py)

set(COPY_DIR_SOURCE "${TEMPLATE_PATH_GAMEDATA_SRC}/shaders")
set(COPY_DIR_TARGET "${TEMPLATE_PATH_GAMEDATA_DST}/shaders")
configure_file("CMake/templates/copydir.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/updateShaders.py)

configure_file("CMake/templates/loc.py.in" ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/loc.py)