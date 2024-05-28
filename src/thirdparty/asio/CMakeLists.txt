include(${top_cmake_dir}/utils.cmake)

set(project_name asio)

project(${project_name})

option(ASIO_BUILD_SHARED_LIBS "Build asio as a shared library" ON)

include_directories(${top_thirdparty_dir}/${project_name}/include)

search_file(${top_thirdparty_dir}/${project_name}/src files)

if(ASIO_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${project_name} SHARED ${files})
    target_compile_definitions(${project_name} PRIVATE ASIO_DYN_LINK)
else()
    add_library(${project_name} STATIC ${files})
endif()
