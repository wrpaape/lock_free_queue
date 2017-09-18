cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(
    single_writer_ring_buffer
    VERSION 0.1.0
    LANGUAGES CXX
)

# cmake options
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD     11)
option(BUILD_TESTING "Enable unit testing." ON)

# config defines
set(SINGLE_WRITER_RING_BUFFER_AUTHOR  "\"wrpaape\"")
set(SINGLE_WRITER_RING_BUFFER_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

# global directories
set(PROJECT_SRC_DIR          ${PROJECT_SOURCE_DIR}/src)
set(PROJECT_INCLUDE_DIR      ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_BUILD_DIR        ${PROJECT_SOURCE_DIR}/build)
set(PROJECT_BIN_DIR          ${PROJECT_SOURCE_DIR}/bin)
set(PROJECT_LIB_DIR          ${PROJECT_SOURCE_DIR}/lib)
set(PROJECT_TEST_DIR         ${PROJECT_SOURCE_DIR}/test)
set(PROJECT_CMAKE_DIR        ${PROJECT_SOURCE_DIR}/cmake)
set(PROJECT_CMAKE_MODULE_DIR ${PROJECT_CMAKE_DIR}/module)

# make cmake modules available to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${PROJECT_CMAKE_MODULE_DIR})

# set up common cmake module environment
include(cmake_environ)

# set up project testing environment
# to disable unit testing, invoke cmake with '-DBUILD_TESTING=OFF' from the
# command line or set BUILD_TESTING to OFF in the interactive GUI (if supported)
if(BUILD_TESTING)
    include(test_environ)
    add_subdirectory(test)
endif()

# enter
add_subdirectory(src)
