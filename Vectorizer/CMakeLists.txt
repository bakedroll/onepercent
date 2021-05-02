add_source_directory(VECTORIZER_SOURCE src)

# add executable Vectorizer
add_executable(Vectorizer ${VECTORIZER_SOURCE})

set_target_properties(Vectorizer PROPERTIES DEBUG_POSTFIX "d")

# specify needed libraries
set(OSG_LIBRARIES_TMP "osg osgUtil osgText OpenThreads")

get_libraries(OSG_LIBRARIES ${OSG_LIBRARY_RELEASE} "" ${OSG_LIBRARIES_TMP})
get_libraries(OSG_LIBRARIES_DEBUG ${OSG_LIBRARY_DEBUG} ${OSG_LIBRARY_SUFFIX} ${OSG_LIBRARIES_TMP})

set(OPENCV_LIBRARIES_TMP "opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc")

get_libraries(OPENCV_LIBRARIES ${OPENCV_LIB_DIR_OPT} "${OPENCV_VERSION_SUFFIX}" ${OPENCV_LIBRARIES_TMP})
get_libraries(OPENCV_LIBRARIES_DEBUG ${OPENCV_LIB_DIR_DBG} "${OPENCV_VERSION_SUFFIX}${OPENCV_LIBRARY_SUFFIX}" ${OPENCV_LIBRARIES_TMP})

# set include directories
target_include_directories(Vectorizer PUBLIC
  ${OPENCV_INCLUDE_DIR}
  ${OSG_INCLUDE_DIR}
  ${OSGHELPER_INCLUDE_DIR}
  ${VECTORIZER_LIB_INCLUDE_DIR})
  
# set debug and release libs
target_link_libraries(Vectorizer
  VectorizerLib
  osgHelper)

set(ALL_LIBRARIES ${OSG_LIBRARIES} ${OPENCV_LIBRARIES})
set(ALL_LIBRARIES_DEBUG ${OSG_LIBRARIES_DEBUG} ${OPENCV_LIBRARIES_DEBUG})
  
foreach(lib ${ALL_LIBRARIES})
  target_link_libraries(Vectorizer
    optimized ${lib})
endforeach()

foreach(lib ${ALL_LIBRARIES_DEBUG})
  target_link_libraries(Vectorizer
    debug ${lib})
endforeach()
  
# copy dlls
if (WIN32 AND EXISTS ${OPENCV_BIN_DIR_OPT})
  copy_dlls(${OPENCV_BIN_DIR_OPT} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "opencv_" "${OPENCV_VERSION_SUFFIX}"
    "core highgui imgcodecs imgproc videoio")
  
  if (OPENCV_LIBRARIES_DEBUG_SUFFIX AND EXISTS ${OPENCV_BIN_DIR_DBG})
    copy_dlls(${OPENCV_BIN_DIR_DBG} ${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR} "opencv_" "${OPENCV_VERSION_SUFFIX}${OPENCV_LIBRARY_SUFFIX}"
      "core highgui imgcodecs imgproc videoio")
  endif()
endif()

# copy debug args file
string(REPLACE "/" "\\" CMAKE_SOURCE_DIR_BACKSLASH ${CMAKE_SOURCE_DIR})
configure_file("${CMAKE_SOURCE_DIR}/CMake/templates/VectorizerDebugArgs.txt.in" "${CMAKE_BINARY_DIR}/${PROJECT_BIN_DIR}/VectorizerDebugArgs.txt")