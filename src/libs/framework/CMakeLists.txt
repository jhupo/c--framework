include(${top_cmake_dir}/utils.cmake)

set(project_name framework)

project(${project_name})

option(FRAMEWORK_BUILD_SHARED_LIBS "Build shared libraries" ON)

include_directories(
    ${top_libs_dir}/${project_name}
    ${top_libs_dir}/common
)

link_directories(
    ${project_name}
    ${top_binary_dir}
)

search_file(${top_libs_dir}/${project_name} files)

if(FRAMEWORK_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${project_name} SHARED ${files})
    target_compile_definitions(${project_name} PUBLIC FRAMEWORK_LIBRARY)
else()
    add_library(${project_name} STATIC ${files})
    target_compile_definitions(${project_name} PUBLIC FRAMEWORK_STATIC_LIBRARY)
endif()

target_link_libraries(
    ${project_name}
    common
)
