add_source_directory(VECTORIZER_LIB_SOURCE src)
add_source_directory(VECTORIZER_LIB_SOURCE include/Vectorizer)

set(VECTORIZER_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# add library VectorizerLib
add_library(VectorizerLib ${VECTORIZER_LIB_SOURCE})

set_target_properties(VectorizerLib PROPERTIES DEBUG_POSTFIX "d")

# set include directories
target_include_directories(VectorizerLib PUBLIC
  ${OPENCV_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
  ${OSGHELPER_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
  
# make variable globally accessable
set(VECTORIZER_LIB_INCLUDE_DIR ${VECTORIZER_LIB_INCLUDE_DIR} PARENT_SCOPE)