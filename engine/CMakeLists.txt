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

set(ENGINE_PLUGINS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/plugins")
set_property(GLOBAL PROPERTY ROME_ENGINE_PLUGIN_TARGETS "")

function(add_engine_plugin plugin_dir)
  get_filename_component(target "${plugin_dir}" NAME)

  file(GLOB_RECURSE plugin_sources CONFIGURE_DEPENDS "${plugin_dir}/src/*.cpp"
       "${plugin_dir}/src/*.cxx" "${plugin_dir}/src/*.cc")
  file(GLOB_RECURSE plugin_headers CONFIGURE_DEPENDS
       "${plugin_dir}/include/*.hpp" "${plugin_dir}/include/*.h")

  list(FIND plugin_sources "${plugin_dir}/src/plugin.cpp" plugin_entrypoint_index)
  if(plugin_entrypoint_index EQUAL -1)
    message(FATAL_ERROR "Plugin ${target} must define src/plugin.cpp")
  endif()

  add_library(${target} SHARED ${plugin_sources} ${plugin_headers})

  target_compile_definitions(${target} PRIVATE RM_ASSERTS_ON)

  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${target} PRIVATE -O3)
    target_compile_definitions(${target} PRIVATE NDEBUG)
  else()
    target_compile_options(${target} PRIVATE -g)
    target_compile_definitions(${target} PRIVATE RM_DEBUG_ON)
  endif()

  target_include_directories(${target} PUBLIC "${plugin_dir}/include")
  target_link_libraries(${target} PUBLIC core::core)

  set_target_properties(
    ${target}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${plugin_dir}/bin"
               LIBRARY_OUTPUT_DIRECTORY "${plugin_dir}/bin"
               ARCHIVE_OUTPUT_DIRECTORY "${plugin_dir}/bin")

  set_property(GLOBAL APPEND PROPERTY ROME_ENGINE_PLUGIN_TARGETS "${target}")
endfunction()

if(EXISTS "${ENGINE_PLUGINS_DIR}")
  file(GLOB engine_plugin_dirs CONFIGURE_DEPENDS LIST_DIRECTORIES true
       "${ENGINE_PLUGINS_DIR}/*")
  foreach(plugin_dir IN LISTS engine_plugin_dirs)
    if(IS_DIRECTORY "${plugin_dir}" AND EXISTS "${plugin_dir}/src/plugin.cpp")
      add_engine_plugin("${plugin_dir}")
    endif()
  endforeach()
endif()
