cmake_minimum_required(VERSION 3.10)
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CORE_PREFIX
    ""
    CACHE PATH "Path to core install prefix (dist folder)")
if(NOT CORE_PREFIX)
  message(
    FATAL_ERROR
      "CORE_PREFIX is not set. Configure with -DCORE_PREFIX=/path/to/dist")
endif()

find_package(core CONFIG REQUIRED PATHS "${CORE_PREFIX}")

function(add_rome_engine_plugin target)
  add_library(${target} SHARED ${ARGN})

  target_compile_definitions(${target} PRIVATE RM_EXPORT_ON RM_ASSERTS_ON)

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${target} PRIVATE -O3)
    target_compile_definitions(${target} PRIVATE NDEBUG)
  else()
    target_compile_options(${target} PRIVATE -g)
    target_compile_definitions(${target} PRIVATE RM_DEBUG_ON)
  endif()

  target_include_directories(${target}
                             PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
  target_link_libraries(${target} PUBLIC core::core)

  set_target_properties(
    ${target}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
               ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endfunction()

add_rome_engine_plugin(engine_physics_newton
                       "${CMAKE_CURRENT_SOURCE_DIR}/src/physics/newton.cpp")
