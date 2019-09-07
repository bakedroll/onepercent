add_source_directory(VECTORIZER_LIB_TEST_SOURCE test)

add_executable(VectorizerLibTest ${VECTORIZER_LIB_TEST_SOURCE})

set_target_properties(VectorizerLibTest PROPERTIES DEBUG_POSTFIX "d")

# specify needed osg libraries
set(VTEST_OSG_LIBRARIES_TMP "osg osgUtil osgText OpenThreads")

get_libraries(VTEST_OSG_LIBRARIES ${OSG_LIBRARY} "" ${VTEST_OSG_LIBRARIES_TMP})
get_libraries(VTEST_OSG_LIBRARIES_DEBUG ${OSG_LIBRARY_DEBUG} ${OSG_LIBRARY_SUFFIX} ${VTEST_OSG_LIBRARIES_TMP})

set(VTEST_OPENCV_LIBRARIES_TMP "opencv_core opencv_highgui opencv_imgcodecs opencv_imgproc")

get_libraries(VTEST_OPENCV_LIBRARIES ${OPENCV_LIB_DIR_OPT} "${OPENCV_VERSION_SUFFIX}" ${VTEST_OPENCV_LIBRARIES_TMP})
get_libraries(VTEST_OPENCV_LIBRARIES_DEBUG ${OPENCV_LIB_DIR_DBG} "${OPENCV_VERSION_SUFFIX}${OPENCV_LIBRARY_SUFFIX}" ${VTEST_OPENCV_LIBRARIES_TMP})


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GTEST_INCLUDE_DIRS}
  ${OSGGAMING_INCLUDE_DIR}
  ${VECTORIZER_LIB_INCLUDE_DIR})

target_link_libraries(VectorizerLibTest
  osgGaming
  VectorizerLib
  ${GTEST_BOTH_LIBRARIES})

set(VTEST_ALL_LIBRARIES ${VTEST_OSG_LIBRARIES} ${VTEST_OPENCV_LIBRARIES})
set(VTEST_ALL_LIBRARIES_DEBUG ${VTEST_OSG_LIBRARIES_DEBUG} ${VTEST_OPENCV_LIBRARIES_DEBUG})
  
foreach(lib ${VTEST_ALL_LIBRARIES})
  target_link_libraries(VectorizerLibTest
    optimized ${lib})
endforeach()

foreach(lib ${VTEST_ALL_LIBRARIES_DEBUG})
  target_link_libraries(VectorizerLibTest
    debug ${lib})
endforeach()
