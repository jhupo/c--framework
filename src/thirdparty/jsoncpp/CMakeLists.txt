include(${top_cmake_dir}/utils.cmake)

set(project_name jsoncpp)

project(${project_name})

option(JSONCPP_BUILD_SHARED_LIBS "Build jsoncpp as a shared library" ON)

search_file(${top_thirdparty_dir}/${project_name} files)

if(JSONCPP_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${project_name} SHARED ${files})
    target_compile_definitions(${project_name} PUBLIC JSON_DLL_BUILD)
else()
    add_library(${project_name} STATIC ${files})
endif()