cmake_policy(SET CMP0043 OLD)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(PROJECT_NAME OnePercent)

add_source_directory(ONE_PERCENT_SOURCE core)
add_source_directory(ONE_PERCENT_SOURCE data)
add_source_directory(ONE_PERCENT_SOURCE nodes)
add_source_directory(ONE_PERCENT_SOURCE simulation)
add_source_directory(ONE_PERCENT_SOURCE states)
add_source_directory(ONE_PERCENT_SOURCE widgets)

add_other_directory(ONE_PERCENT_SOURCE GameData/CSS)
add_other_directory(ONE_PERCENT_SOURCE GameData/shaders)

# add executable OnePercent
add_executable(${PROJECT_NAME} ${ONE_PERCENT_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

qt5_use_modules(${PROJECT_NAME} ${QT5_MODULES})
source_group(GeneratedFiles FILES ${CMAKE_CURRENT_BINARY_DIR}/OnePercent_automoc.cpp)

# specify needed osg libraries
set(OSG_LIBRARIES_TMP "osg osgViewer osgUtil osgGA osgDB osgText OpenThreads")

get_libraries(OSG_LIBRARIES ${OSG_LIBRARY} "" ${OSG_LIBRARIES_TMP})
get_libraries(OSG_LIBRARIES_DEBUG ${OSG_LIBRARY_DEBUG} ${OSG_LIBRARY_SUFFIX} ${OSG_LIBRARIES_TMP})

# set include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/osgGaming/include
  ${OSG_INCLUDE_DIR}
  ${RAPIDXML_INCLUDE_DIR}
  ${OSGPPU_INCLUDE_DIR})

# set debug and release libs
target_link_libraries(${PROJECT_NAME}
  osgGaming)

target_link_libraries(${PROJECT_NAME}
  optimized ${OSGPPU_LIBRARY_RELEASE})
  
target_link_libraries(${PROJECT_NAME}
  debug ${OSGPPU_LIBRARY_DEBUG})

if (MSVC)
  foreach(lib ${OSG_LIBRARIES})
    target_link_libraries(${PROJECT_NAME}
      optimized ${lib})
  endforeach()

  foreach(lib ${OSG_LIBRARIES_DEBUG})
    target_link_libraries(${PROJECT_NAME}
      debug ${lib})
  endforeach()
else()
  target_link_libraries(${PROJECT_NAME} ${OPENSCENEGRAPH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

# copy GameData
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/GameData DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# post-build refresh stylesheets and shaders
file(GLOB FILES_STYLESHEETS "${CMAKE_CURRENT_SOURCE_DIR}/GameData/CSS/*.css")
file(GLOB FILES_SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/GameData/shaders/*.*")

message(STATUS ${FILES_STYLESHEETS})
message(STATUS ${FILES_SHADERS})

foreach(file ${FILES_STYLESHEETS})
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${file} ${CMAKE_CURRENT_BINARY_DIR}/GameData/CSS
  )
endforeach()

foreach(file ${FILES_SHADERS})
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${file} ${CMAKE_CURRENT_BINARY_DIR}/GameData/shaders
  )
endforeach()

# configure launch script
configure_file("../CMake/templates/make_run.sh.in" "${CMAKE_BINARY_DIR}/make_run.sh")

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)