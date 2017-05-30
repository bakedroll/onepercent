add_source_directory(OSG_GAMING_SOURCE src)
add_source_directory(OSG_GAMING_SOURCE include)

set(OSGGAMING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# add library osgGaming
add_library(osgGaming ${OSG_GAMING_SOURCE})

set_target_properties(osgGaming PROPERTIES DEBUG_POSTFIX "d")
target_compile_options(osgGaming PRIVATE "-fpermissive")

# set include directories
target_include_directories(osgGaming PUBLIC
  ${OSGGAMING_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
  ${RAPIDXML_INCLUDE_DIR})
  
# make variable globally accessable
set(OSGGAMING_INCLUDE_DIR ${OSGGAMING_INCLUDE_DIR} PARENT_SCOPE)