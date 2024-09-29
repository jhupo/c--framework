cmake_minimum_required(VERSION 3.0)

project(framework VERSION 0.1.0 LANGUAGES CXX)

include(cmake/utils.cmake)
include(cmake/install.cmake)

option(BUILD_TESTS "Build tests" OFF)

if(MSVC)
    string(APPEND CMAKE_CXX_FLAGS " /MP")
endif()

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)  
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

configure_file(
    cmake/template.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h
)

find_package(Threads REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_target_or_filter(
    FOUND_FILES 
    FOLDERS ${CMAKE_SOURCE_DIR}/include 
            ${CMAKE_SOURCE_DIR}/src 
    FILTERS "*.h;*.hpp;*.cpp;*.c"
)

set_files_filter_group(${FOUND_FILES})

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${FOUND_FILES})
    target_compile_definitions(-DFRANEWORK_LIBRARY)
else()
    add_library(${PROJECT_NAME} STATIC ${FOUND_FILES})
endif()


disable_warnings()

target_compile_definitions(
    ${PROJECT_NAME} 
    PUBLIC $<$<PLATFORM_ID:Windows>:_UNICODE UNICODE> 
    PUBLIC FRAMEWORK_GLOBAL_NAMESPACE=framework
)

target_include_directories(
    ${PROJECT_NAME}  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

install_library(${PROJECT_NAME} ${PROJECT_VERSION} ${CMAKE_CURRENT_SOURCE_DIR}/include/)



