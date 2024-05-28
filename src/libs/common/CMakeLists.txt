include(${top_cmake_dir}/utils.cmake)

set(project_name common)

project(${project_name})

option(COMMON_BUILD_SHARED_LIBS "Build common as a shared library" ON)

include_directories(
    ${top_libs_dir}/${project_name}
    ${top_thirdparty_dir}/jsoncpp
    ${top_thirdparty_dir}/spdlog/include
)

link_directories(
    ${project_name}
    ${top_binary_dir}
)

search_file(${top_libs_dir}/${project_name} files)

if(COMMON_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${project_name} SHARED ${files})
    target_compile_definitions(${project_name} PUBLIC COMMON_LIBRARY)
else()
    add_library(${project_name} STATIC ${files})
    target_compile_definitions(${project_name} PUBLIC COMMON_STATIC_LIBRARY)
endif()

target_link_libraries(
    ${project_name}
    jsoncpp
    spdlog
    sqlite3
)
