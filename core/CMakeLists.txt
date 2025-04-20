cmake_minimum_required(VERSION 3.14)

project(core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build unit tests" OFF)

file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if(BUILD_TESTS)
    list(REMOVE_ITEM CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/entry/entry.cpp)
endif()

add_library(core STATIC ${CORE_SOURCES})

target_compile_definitions(core PRIVATE
    IO_EXPORT_ON
    IO_ASSERTS_ON
    IO_EXCEPTIONS_ON
)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    target_compile_definitions(core PRIVATE
        IO_DEBUG_ON
    )
endif()

target_include_directories(core 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL/include
)

set_target_properties(core PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

if(APPLE)
  target_link_libraries(
    core
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/SDL/build/libSDL3.a
            "-framework Cocoa"
            "-framework Foundation"
            "-framework CoreAudio"
            "-framework IOKit"
            pthread
            dl
            m
            )
endif()

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)

    add_executable(core_tests ${TEST_SOURCES})
    target_link_libraries(core_tests PRIVATE core gtest gtest_main)
    add_test(NAME CoreTests COMMAND core_tests)
endif()