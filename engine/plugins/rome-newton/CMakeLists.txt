cmake_minimum_required(VERSION 3.14)

get_filename_component(PLUGIN_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
project(${PLUGIN_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CORE_PREFIX
    ""
    CACHE PATH "Path to core install prefix (dist folder)")
if(NOT CORE_PREFIX)
  message(
    FATAL_ERROR
      "CORE_PREFIX is not set. Configure with -DCORE_PREFIX=/path/to/dist")
endif()

find_package(core CONFIG REQUIRED PATHS "${CORE_PREFIX}" NO_DEFAULT_PATH)

file(
  GLOB_RECURSE
  PLUGIN_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE PLUGIN_HEADERS CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

list(FIND PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/plugin.cpp"
     PLUGIN_ENTRYPOINT_INDEX)
if(PLUGIN_ENTRYPOINT_INDEX EQUAL -1)
  message(FATAL_ERROR "Plugins must define src/plugin.cpp")
endif()

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE RM_ASSERTS_ON)
target_include_directories(${PROJECT_NAME}
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PUBLIC core::core)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
  if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /O2)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
  endif()
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE RM_DEBUG_ON)
  if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
  endif()
endif()

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             BUILD_RPATH "${CORE_PREFIX}/lib"
             INSTALL_RPATH "${CORE_PREFIX}/lib")
