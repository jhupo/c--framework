set(project_name test-asio)

include_directories(
    ${boost_include_dir}
    ${top_thirdparty_dir}/asio/include
)

link_directories(
    ${top_binary_dir}
)

search_file(${top_test_dir}/${project_name} TAGET_FILES)

add_executable(${project_name} ${TAGET_FILES})

set_rpath_target(${project_name})

target_link_libraries(
    ${project_name}
    asio
    pthread
)
