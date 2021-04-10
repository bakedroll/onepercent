set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PROJECT_NAME luadocTest)

add_source_directory(LUADOC_TEST_SOURCE test)

add_executable(${PROJECT_NAME} ${LUADOC_TEST_SOURCE})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

include_directories(
  ${LUADOC_INCLUDE_DIR}
  ${GTEST_INCLUDE_DIRS}
  ${LUA_INCLUDE_PREFIX}/include
  ${LUA_INCLUDE_DIR}
  ${LUABRIDGE_INCLUDE_DIR}
  ${OSGHELPER_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
  luadoc
  ${GTEST_BOTH_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${Qt${QT_VERSION}Core_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION}::Core)

target_link_libraries(${PROJECT_NAME} optimized ${LUA_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${LUA_LIB_DEBUG})

set(CMAKE_INCLUDE_CURRENT_DIR OFF)