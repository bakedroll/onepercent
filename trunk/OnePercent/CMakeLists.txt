cmake_policy(SET CMP0043 OLD)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_source_directory(ONE_PERCENT_SOURCE core)
add_source_directory(ONE_PERCENT_SOURCE data)
add_source_directory(ONE_PERCENT_SOURCE nodes)
add_source_directory(ONE_PERCENT_SOURCE simulation)
add_source_directory(ONE_PERCENT_SOURCE states)
add_source_directory(ONE_PERCENT_SOURCE widgets)

# add executable OnePercent
add_executable(OnePercent ${ONE_PERCENT_SOURCE})

set_target_properties(OnePercent PROPERTIES DEBUG_POSTFIX "d")

qt5_use_modules(OnePercent ${QT5_MODULES})
source_group(GeneratedFiles FILES ${CMAKE_CURRENT_BINARY_DIR}/OnePercent_automoc.cpp)

# specify needed osg libraries
set(OSG_LIBRARIES_TMP "osg osgViewer osgUtil osgGA osgDB osgText osgPPU OpenThreads")

get_libraries(OSG_LIBRARIES ${OSG_LIBRARY} "" ${OSG_LIBRARIES_TMP})
get_libraries(OSG_LIBRARIES_DEBUG ${OSG_LIBRARY_DEBUG} ${OSG_LIBRARY_SUFFIX} ${OSG_LIBRARIES_TMP})

# set include directories
target_include_directories(OnePercent PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/osgGaming/include
  ${OSG_INCLUDE_DIR}
  ${RAPIDXML_INCLUDE_DIR})

# set debug and release libs
target_link_libraries(OnePercent
  osgGaming)
  
if (MSVC)
  foreach(lib ${OSG_LIBRARIES})
    target_link_libraries(OnePercent
      optimized ${lib})
  endforeach()

  foreach(lib ${OSG_LIBRARIES_DEBUG})
    target_link_libraries(OnePercent
      debug ${lib})
  endforeach()
else()
  #TODO: write find script for osgPPU
  target_link_libraries(OnePercent ${OPENSCENEGRAPH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} /usr/local/lib64/libosgPPU.so.0.5.9)
endif()

# copy GameData
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/GameData DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)