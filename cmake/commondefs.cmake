set(top_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(top_binary_dir ${top_dir}/bin)
set(top_build_dir ${top_dir}/build)
set(top_framework_include ${top_dir}/include)
set(top_3rdparty_dir ${top_dir}/3rdparty)

function(search_file dir result)
    file(GLOB_RECURSE __FILES_TARGET__
        "${dir}/*.hpp"
        "${dir}/*.h"
        "${dir}/*.cpp"
        "${dir}/*.cc"
        "${dir}/*.c"
    )
    SET(${result} ${__FILES_TARGET__} PARENT_SCOPE)
endfunction()

function(set_rpath_target target)
    set_target_properties(${target} PROPERTIES BUILD_RPATH "$ORIGIN")
endfunction()

if(NOT cmake_build_type)
    set(cmake_build_type "Debug")
endif()

message(STATUS "cmake_build_type: ${cmake_build_type}")

set(cmake_cxx_flags_debug "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(cmake_cxx_flags_release "$ENV{CXXFLAGS} -DNODEBUG -O2 -Wall")
set(cmake_cxx_flags_relwithdebinfo "$ENV{CXXFLAGS} -Wall -O2 -g -DNDEBUG")

set(cmake_c_flags_debug "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
set(cmake_c_flags_release "$ENV{CFLAGS} -DNODEBUG -O2 -Wall")
set(cmake_c_flags_relwithdebinfo "$ENV{CXXFLAGS} -Wall -O2 -g -DNDEBUG")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(thirdparty_libs 
                m
                dl
                pthread)
