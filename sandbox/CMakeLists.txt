cmake_minimum_required(VERSION 3.10)

project(sandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


file(GLOB_RECURSE SANDBOX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(sandbox ${SANDBOX_SOURCES})

target_compile_definitions(sandbox PRIVATE
    IO_EXPORT_ON
    IO_ASSERTS_ON
)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    target_compile_definitions(sandbox PRIVATE
        IO_DEBUG_ON
    )
endif()

target_include_directories(sandbox PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../core/src) # Kind of a hack, but it works

set(CORE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../core/bin/libcore.a)
target_link_libraries(sandbox PRIVATE ${CORE_LIB})

set_target_properties(sandbox PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)