set(PROJECT_NAME osgGaming)

add_source_directory(OSGGAMING_SOURCE src)
add_source_directory(OSGGAMING_SOURCE include)

set(OSGGAMING_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} ${OSGGAMING_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
target_compile_options(${PROJECT_NAME} PRIVATE "-fpermissive")

# set include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${OSGGAMING_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
  ${OSGPPU_INCLUDE_DIR})
  
# make variable globally accessable
set(OSGGAMING_INCLUDE_DIR ${OSGGAMING_INCLUDE_DIR} PARENT_SCOPE)