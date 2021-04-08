set(CMAKE_AUTORCC ON)

set(PROJECT_NAME luadoc)

add_source_directory(LUADOC_SOURCE src)
add_source_directory(LUADOC_SOURCE include)

set(LUADOC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# add library luadoc
add_library(${PROJECT_NAME}
  ${LUADOC_SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/luadoc_resources.qrc)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

source_group(Resources FILES ${CMAKE_CURRENT_SOURCE_DIR}/luadoc_resources.qrc)

# set include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${LUADOC_INCLUDE_DIR}
  ${LUA_INCLUDE_PREFIX}/include
  ${LUA_INCLUDE_DIR}
  ${LUABRIDGE_INCLUDE_DIR}
  ${Qt5Core_INCLUDE_DIRS}
  ${OSGHELPER_INCLUDE_DIR})

set(CMAKE_AUTORCC OFF)

# make variable globally accessable
set(LUADOC_INCLUDE_DIR ${LUADOC_INCLUDE_DIR} PARENT_SCOPE)