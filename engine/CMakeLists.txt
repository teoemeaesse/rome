cmake_minimum_required(VERSION 3.10)

# Set up the project
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


file(GLOB_RECURSE ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(engine SHARED ${ENGINE_SOURCES})

target_compile_definitions(engine PRIVATE
    IO_EXPORT_ON
    IO_ASSERTS_ON
)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    target_compile_definitions(engine PRIVATE
        IO_DEBUG_ON
    )
endif()

target_include_directories(engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/src) # Kind of a hack, but it works


set(CORE_LIB_PATH "" CACHE PATH "Path to the core shared library")
if(NOT CORE_LIB_PATH)
    message(FATAL_ERROR "CORE_LIB_PATH is not set. Specify the path to the core shared library when calling cmake (e.g., -DCORE_LIB_PATH=/path/to/core/bin).")
endif()
find_library(CORE_LIB core PATHS ${CORE_LIB_PATH})
if(NOT CORE_LIB)
    message(FATAL_ERROR "Core library not found in the specified path: ${CORE_LIB_PATH}")
endif()

target_link_libraries(engine PRIVATE ${CORE_LIB})

set_target_properties(engine PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)