set(top_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(top_binary_dir ${top_dir}/bin)
set(top_build_dir ${top_dir}/build)
set(top_cmake_dir ${top_dir}/cmake)
set(top_3rdparty_dir ${top_dir}/3rdparty)
set(top_examples_dir ${top_dir}/examples)
set(top_test_dir ${top_dir}/test)
set(top_src_dir ${top_dir}/src)
set(top_libs_dir ${top_src_dir}/libs)
set(top_plugins_dir ${top_src_dir}/plugins)
set(top_thirdparty_dir ${top_src_dir}/thirdparty)
set(top_exec_dir ${top_src_dir}/exec)

if(NOT GLOBAL_NAMESPACE)
    add_definitions(-DGLOBAL_NAMESPACE=framework)
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER 1700)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++11")
    endif()
endif()

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${top_binary_dir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${top_binary_dir})

if(MSVC)
    string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus /MP")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

