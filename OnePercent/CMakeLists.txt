begin_project(OnePercent EXECUTABLE)

enable_automoc()
enable_autorcc()

require_library(OpenSceneGraph MODULES osg osgViewer osgUtil osgGA osgDB osgText OpenThreads)
require_library(osgPPU)
require_library(Qt6 MODULES Core Gui Widgets OpenGL OpenGLWidgets)
require_library(Lua)
require_library(LuaBridge)

require_project(osgHelper PATH osgHelper)
require_project(luaHelper PATH luaHelper)
require_project(QtOsgBridge PATH QtOsgBridge)

if (LUADOC_ENABLE)
  add_project_definitions(
    -DLUADOC_ENABLED
    -DLUADOC_DESCRIPTIONS_FILE="${CMAKE_SOURCE_DIR}/Resources/luaapi.csv")

  require_project(luadoc PATH luadoc)
  add_required_project(luadocTest PATH luadoc)
endif()

add_source_directory(core)
add_source_directory(data)
add_source_directory(nodes)
add_source_directory(scripting)
add_source_directory(simulation)
add_source_directory(states)
add_source_directory(widgets)

add_other_directory(GameData/shaders)
add_other_directory(GameData/scripts)

add_qrc_files(resources.qrc)

# copy GameData
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/GameData DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Mods DESTINATION ${CMAKE_BINARY_DIR})
