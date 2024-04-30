cmake_minimum_required(VERSION 3.1)

include(cmake/commondefs.cmake)

set(project_name framework)

project(${project_name})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${top_binary_dir})

include_directories(
    ${top_framework_include}
)

search_file(${top_dir}/src TAGET_FILES)

add_library(${project_name} SHARED ${TAGET_FILES})

add_definitions(-DFRAMEWORK_LIBRARY)

target_link_libraries(${project_name} 
    ${thirdparty_libs}
    spdlog
    jsoncpp
)
