if(PROJECT_CMAKE_ADD_GOOGLE_TEST_CMAKE_INCLUDED)
    return()
endif()
set(PROJECT_CMAKE_ADD_GOOGLE_TEST_CMAKE_INCLUDED TRUE)
# ==============================================================================
# add a 'test_${NAME}' executable and CTest from sources compiled with the
# Google Test/Mock framework
# ==============================================================================
# External Dependencies
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR) # ExternalProject
find_package(Threads REQUIRED)                    # We need thread support
include(ExternalProject)
include(add_custom_test)


# Exported Variables
# ------------------------------------------------------------------------------
set(GOOGLE_TEST_SRC_DIR     ${PROJECT_TEST_SRC_DIR}/googletest)
set(GOOGLE_TEST_INCLUDE_DIR ${PROJECT_TEST_INCLUDE_DIR}/googletest)
set(GOOGLE_TEST_BUILD_DIR   ${PROJECT_TEST_BUILD_DIR}/googletest)
set(
    GOOGLE_TEST_LIBRARIES
    gtest
    gtest_main
    gmock
    gmock_main
)
 

# Set Up Framework
# ------------------------------------------------------------------------------
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        master
    SOURCE_DIR     ${GOOGLE_TEST_SRC_DIR}
    BINARY_DIR     ${GOOGLE_TEST_BUILD_DIR}
    INSTALL_DIR    ${PROJECT_TEST_DIR}
    CMAKE_ARGS     -DCMAKE_INSTALL_PREFIX=${PROJECT_TEST_DIR}
)

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# import libraries from installed location
foreach(library ${GOOGLE_TEST_LIBRARIES})
    add_library(${library} STATIC IMPORTED GLOBAL)
    set(filename
        ${CMAKE_STATIC_LIBRARY_PREFIX}${library}${CMAKE_STATIC_LIBRARY_SUFFIX})
    set_target_properties(
        ${library}
        PROPERTIES
        IMPORTED_LOCATION                 ${PROJECT_TEST_LIB_DIR}/${filename}
        INTERFACE_INCLUDE_DIRECTORIES     ${PROJECT_TEST_INCLUDE_DIR}
        IMPORTED_LINK_INTERFACE_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
    )
    add_dependencies(${library} googletest)
endforeach()



# External API
# ------------------------------------------------------------------------------
function(add_google_test)
    add_custom_test(
        ${ARGV}
        FRAMEWORK_NAME      googletest
        FRAMEWORK_LIBRARIES ${GOOGLE_TEST_LIBRARIES}
    )
endfunction()
