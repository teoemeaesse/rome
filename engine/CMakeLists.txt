cmake_minimum_required(VERSION 3.10)
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx")

if(ENGINE_SOURCES STREQUAL "")
  message(
    FATAL_ERROR "No engine sources found under: ${CMAKE_CURRENT_SOURCE_DIR}/src"
  )
endif()

add_library(engine SHARED ${ENGINE_SOURCES})

target_compile_definitions(engine PRIVATE RM_EXPORT_ON RM_ASSERTS_ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  target_compile_definitions(engine PRIVATE RM_DEBUG_ON)
endif()

target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CORE_PREFIX
    ""
    CACHE PATH "Path to core install prefix (dist folder)")
if(NOT CORE_PREFIX)
  message(
    FATAL_ERROR
      "CORE_PREFIX is not set. Configure with -DCORE_PREFIX=/path/to/dist")
endif()

find_package(core CONFIG REQUIRED PATHS "${CORE_PREFIX}")
target_link_libraries(engine PUBLIC core::core)

set_target_properties(
  engine
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
