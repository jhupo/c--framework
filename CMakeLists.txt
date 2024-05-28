cmake_minimum_required(VERSION 3.1)

project(framework-cpp)

include(cmake/commondefs.cmake)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_TESTS "Build test executable" OFF)
option(BUILD_EXAMPLES "Build example executable" OFF)

add_subdirectory(${top_src_dir})

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()