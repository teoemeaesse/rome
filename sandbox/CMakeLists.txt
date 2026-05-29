cmake_minimum_required(VERSION 3.10)
project(sandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SANDBOX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(APPEND SANDBOX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../core/src/entry/entry.cpp)
add_executable(sandbox ${SANDBOX_SOURCES})

target_compile_definitions(sandbox PRIVATE RM_EXPORT_ON RM_ASSERTS_ON)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  target_compile_definitions(sandbox PRIVATE RM_DEBUG_ON)
endif()

target_include_directories(sandbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CORE_PREFIX
    ""
    CACHE PATH "Path to core install prefix (dist folder)")
if(NOT CORE_PREFIX)
  message(FATAL_ERROR "Set -DCORE_PREFIX=/path/to/dist")
endif()

find_package(core CONFIG REQUIRED PATHS "${CORE_PREFIX}")
add_subdirectory(../engine ${CMAKE_CURRENT_BINARY_DIR}/engine_build)

target_link_libraries(sandbox PRIVATE core::core)
target_include_directories(sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../engine/include)
target_compile_definitions(sandbox
                           PRIVATE RM_NEWTON_PLUGIN_PATH="$<TARGET_FILE:engine_physics_newton>")
add_dependencies(sandbox engine_physics_newton)

set_target_properties(
  sandbox
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
             ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
