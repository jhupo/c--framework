include(${top_cmake_dir}/utils.cmake)

set(project_name sqlite3)

project(${project_name})

option(SQLITE3_BUILD_SHARED_LIBS "Build sqlite3 as a shared library" ON)

search_file(${top_thirdparty_dir}/${project_name} files)

if(SQLITE3_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${project_name} SHARED ${files})
else()
    add_library(${project_name} STATIC ${files})
endif()