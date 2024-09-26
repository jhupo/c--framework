# InstallLibrary.cmake

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(cmake_utils_dir ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

function(install_library TARGET_NAME PROJECT_VERSION INCLUDE_DIR)
    # Set the PACKAGE_INCLUDE_INSTALL_DIR variable
    set(PACKAGE_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})

    # Generate the config file
    configure_package_config_file(
        "${cmake_utils_dir}/config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_NAME}
    )

    # Generate the version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Install the shared library
    install(
        TARGETS ${TARGET_NAME}
        EXPORT ${TARGET_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}    # For non-Windows platforms
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}    # For static libraries
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}    # For Windows DLLs
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # For headers
    )

    # Install the header files
    install(
        DIRECTORY ${INCLUDE_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Install the export file
    install(
        EXPORT ${TARGET_NAME}-targets
        FILE ${TARGET_NAME}-targets.cmake
        NAMESPACE ${TARGET_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_NAME}
    )

    # Install the configuration files
    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_NAME}
    )

    # Install the PDB files for debugging
    if(MSVC)
        install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> DESTINATION bin OPTIONAL)
    endif()
    
endfunction()

function(install_3rdparty TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
        $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PDB_FILE:${TARGET_NAME}>
        $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMAND_EXPAND_LISTS
    )
    set_target_properties(${TARGET_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
    )
endfunction()

