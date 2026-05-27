cmake_minimum_required(VERSION 3.14)

project(core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_SHARED_CORE "Build core as a shared library" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if(BUILD_TESTS)
  list(REMOVE_ITEM CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/entry/entry.cpp)
endif()

if(BUILD_SHARED_CORE)
  add_library(core SHARED ${CORE_SOURCES})
else()
  add_library(core STATIC ${CORE_SOURCES})
endif()
add_library(core::core ALIAS core)

set_target_properties(core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(core PRIVATE RM_EXPORT_ON RM_ASSERTS_ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  target_compile_definitions(core PRIVATE RM_DEBUG_ON)
endif()

target_include_directories(
  core
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL/include)

if(BUILD_TESTS)
  set(CORE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else()
  set(CORE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()

set_target_properties(
  core
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CORE_OUTPUT_DIRECTORY}
             LIBRARY_OUTPUT_DIRECTORY ${CORE_OUTPUT_DIRECTORY}
             ARCHIVE_OUTPUT_DIRECTORY ${CORE_OUTPUT_DIRECTORY})

if(BUILD_SHARED_CORE)
  set_target_properties(core PROPERTIES
                        MACOSX_RPATH ON
                        BUILD_RPATH "@loader_path;@executable_path"
                        INSTALL_RPATH "@loader_path")
endif()

if(APPLE)
  target_link_libraries(
    core
    PUBLIC "-framework Cocoa"
           "-framework Foundation"
           "-framework CoreAudio"
           "-framework IOKit"
           pthread
           dl
           m
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/SDL/build/libSDL3.a)
endif()

if(UNIX AND NOT APPLE)
  target_link_libraries(core PUBLIC pthread dl m)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
  TARGETS core
  EXPORT coreTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  EXPORT coreTargets
  FILE coreTargets.cmake
  NAMESPACE core::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/core)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/coreConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/coreConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/core)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/coreConfigVersion.cmake
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/coreConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/coreConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/core)

if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
  )
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  enable_testing()

  file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
  list(REMOVE_ITEM TEST_SOURCES
       ${CMAKE_CURRENT_SOURCE_DIR}/tests/plugin/test_plugin.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/tests/plugin/dependency_plugin.cpp
       ${CMAKE_CURRENT_SOURCE_DIR}/tests/plugin/dependent_plugin.cpp)

  add_library(core_test_plugin SHARED
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/plugin/test_plugin.cpp)
  target_link_libraries(core_test_plugin PRIVATE core)
  target_compile_definitions(core_test_plugin PRIVATE RM_ASSERTS_ON RM_DEBUG_ON)
  set_target_properties(
    core_test_plugin
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
               LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
               ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  add_library(core_test_dependency_plugin SHARED
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/plugin/dependency_plugin.cpp)
  target_link_libraries(core_test_dependency_plugin PRIVATE core)
  target_compile_definitions(core_test_dependency_plugin
                             PRIVATE RM_ASSERTS_ON RM_DEBUG_ON)
  set_target_properties(
    core_test_dependency_plugin
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
               LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
               ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  add_library(core_test_dependent_plugin SHARED
              ${CMAKE_CURRENT_SOURCE_DIR}/tests/plugin/dependent_plugin.cpp)
  target_link_libraries(core_test_dependent_plugin PRIVATE core)
  target_compile_definitions(
    core_test_dependent_plugin
    PRIVATE RM_ASSERTS_ON RM_DEBUG_ON
            RM_TEST_DEPENDENCY_PLUGIN_FILE="$<TARGET_FILE_NAME:core_test_dependency_plugin>"
  )
  set_target_properties(
    core_test_dependent_plugin
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
               LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
               ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  add_executable(core_tests ${TEST_SOURCES})
  add_dependencies(core_tests core_test_plugin core_test_dependency_plugin
                   core_test_dependent_plugin)
  target_link_libraries(core_tests PRIVATE core gtest gtest_main)
  target_compile_definitions(core_tests PRIVATE RM_ASSERTS_ON RM_DEBUG_ON
                                                RM_TEST_PLUGIN_PATH="$<TARGET_FILE:core_test_plugin>"
                                                RM_TEST_DEPENDENT_PLUGIN_PATH="$<TARGET_FILE:core_test_dependent_plugin>")
  add_test(NAME CoreTests COMMAND core_tests)
endif()
