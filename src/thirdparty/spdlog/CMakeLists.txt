include(${top_cmake_dir}/utils.cmake)

set(project_name spdlog)

project(${project_name})

option(SPDLOG_BUILD_SHARED_LIBS "Build spdlog as a shared library" ON)

include_directories(${top_thirdparty_dir}/${project_name}/include)

search_file(${top_thirdparty_dir}/${project_name} files)

if(JSONCPP_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${project_name} SHARED ${files})
    target_compile_definitions(${project_name} PUBLIC SPDLOG_SHARED_LIB)
else()
    add_library(${project_name} STATIC ${files})
endif()

target_compile_definitions(${project_name} PUBLIC SPDLOG_COMPILED_LIB)
