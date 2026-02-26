cmake_minimum_required(VERSION 3.10)
project(sandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SANDBOX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
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

add_subdirectory(../engine ${CMAKE_CURRENT_BINARY_DIR}/engine_build)

target_link_libraries(sandbox PRIVATE engine)

set_target_properties(
  sandbox
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
             ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
