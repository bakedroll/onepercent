add_source_directory(OSG_GAMING_TEST_SOURCE mock)
add_source_directory(OSG_GAMING_TEST_SOURCE test)

add_executable(osgGamingTest ${OSG_GAMING_TEST_SOURCE})

set_target_properties(osgGamingTest PROPERTIES DEBUG_POSTFIX "d")

# specify needed osg libraries
set(TEST_OSG_LIBRARIES_TMP "osg osgViewer osgUtil osgGA osgDB osgText osgPPU OpenThreads")

get_libraries(TEST_OSG_LIBRARIES ${OSG_LIBRARY} "" ${TEST_OSG_LIBRARIES_TMP})
get_libraries(TEST_OSG_LIBRARIES_DEBUG ${OSG_LIBRARY_DEBUG} ${OSG_LIBRARY_SUFFIX} ${TEST_OSG_LIBRARIES_TMP})


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${GTEST_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/osgGaming/include)

target_link_libraries(osgGamingTest
  osgGaming
  ${GTEST_BOTH_LIBRARIES})

target_link_libraries(osgGamingTest
  optimized ${OSGPPU_LIBRARY_RELEASE})
  
if (NOT ${OSGPPU_LIBRARY_RELEASE} STREQUAL ${OSGPPU_LIBRARY_DEBUG})
  target_link_libraries(osgGamingTest
    debug ${OSGPPU_LIBRARY_DEBUG})
endif()
  
if (MSVC)
  foreach(lib ${TEST_OSG_LIBRARIES})
    target_link_libraries(osgGamingTest
      optimized ${lib})
  endforeach()

  foreach(lib ${TEST_OSG_LIBRARIES_DEBUG})
    target_link_libraries(osgGamingTest
      debug ${lib})
  endforeach()
else()
  target_link_libraries(osgGamingTest ${CMAKE_THREAD_LIBS_INIT} ${OPENSCENEGRAPH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()