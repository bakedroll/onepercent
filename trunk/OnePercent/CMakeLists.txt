set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_NAME OnePercent)

add_source_directory(ONE_PERCENT_SOURCE core)
add_source_directory(ONE_PERCENT_SOURCE data)
add_source_directory(ONE_PERCENT_SOURCE nodes)
add_source_directory(ONE_PERCENT_SOURCE scripting)
add_source_directory(ONE_PERCENT_SOURCE simulation)
add_source_directory(ONE_PERCENT_SOURCE states)
add_source_directory(ONE_PERCENT_SOURCE widgets)

add_other_directory(ONE_PERCENT_SOURCE GameData/CSS)
add_other_directory(ONE_PERCENT_SOURCE GameData/shaders)
add_other_directory(ONE_PERCENT_SOURCE GameData/scripts)

# add executable OnePercent
add_executable(${PROJECT_NAME}
  ${ONE_PERCENT_SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

if (WIN32)
  if(NOT BUILD_ENABLE_CONSOLE)
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
  endif()
endif()

source_group(GeneratedFiles FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/moc_compilation.cpp)

source_group(Resources FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

# specify needed osg libraries
set(OSG_LIBRARIES_TMP "osg osgViewer osgUtil osgGA osgDB osgText OpenThreads")

get_libraries(OSG_LIBRARIES ${OSG_LIBRARY} "" ${OSG_LIBRARIES_TMP})
get_libraries(OSG_LIBRARIES_DEBUG ${OSG_LIBRARY_DEBUG} ${OSG_LIBRARY_SUFFIX} ${OSG_LIBRARIES_TMP})

# set include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OSG_INCLUDE_DIR}
  ${OSGPPU_INCLUDE_DIR}
  ${LUA_INCLUDE_PREFIX}/include
  ${LUABRIDGE_INCLUDE_DIR})

foreach(module ${QT5_MODULES})
  target_include_directories(${PROJECT_NAME} PUBLIC ${Qt5${module}_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} Qt5::${module})
endforeach()

# set debug and release libs

if (LUADOC_ENABLE)
  add_definitions(
    -DLUADOC_ENABLED
    -DLUADOC_DESCRIPTIONS_FILE="${CMAKE_SOURCE_DIR}/Resources/luaapi.csv")
  
  target_link_libraries(${PROJECT_NAME} luadoc)
endif()

target_link_libraries(${PROJECT_NAME} osgGaming)

target_link_libraries(${PROJECT_NAME}
  optimized ${OSGPPU_LIBRARY_RELEASE} ${LUA_LIBRARY})
  
target_link_libraries(${PROJECT_NAME}
  debug ${OSGPPU_LIBRARY_DEBUG} ${LUA_LIB_DEBUG})

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
file(GLOB FILES_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/GameData/scripts/*.*")

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

foreach(file ${FILES_SCRIPTS})
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${file} ${CMAKE_CURRENT_BINARY_DIR}/GameData/scripts
  )
endforeach()

# configure launch script
configure_file("../CMake/templates/make_run.sh.in" "${CMAKE_BINARY_DIR}/make_run.sh")

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)