cmake_minimum_required(VERSION 3.10)

project(core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build unit tests" OFF)

file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(core STATIC ${CORE_SOURCES})

target_compile_definitions(core PRIVATE
    IO_EXPORT_ON
    IO_ASSERTS_ON
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
            m)
endif()

if(BUILD_TESTS)
    enable_testing()
    include(CTest)

    file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
    add_library(unity STATIC ${CMAKE_CURRENT_SOURCE_DIR}/include/unity/unity.c)
    
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME}_test ${TEST_SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/include/unity/unity.c)
        target_include_directories(${TEST_NAME}_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_link_libraries(${TEST_NAME}_test PRIVATE core)
        set_target_properties(${TEST_NAME}_test PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/tests
        )

        add_test(NAME ${TEST_NAME}_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/tests/${TEST_NAME}_test)
    endforeach()
endif()