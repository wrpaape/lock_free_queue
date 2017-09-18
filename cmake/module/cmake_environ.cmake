if(PROJECT_CMAKE_CMAKE_ENVIRON_CMAKE_INCLUDED)
    return()
endif()
set(PROJECT_CMAKE_CMAKE_ENVIRON_CMAKE_INCLUDED TRUE)
# ==============================================================================
# set up common cmake environment
# ==============================================================================
# Exported Variables
# ------------------------------------------------------------------------------
set(PROJECT_CMAKE_RESOURCE_DIR ${PROJECT_CMAKE_DIR}/resource)
set(PROJECT_CMAKE_SRC_DIR      ${PROJECT_CMAKE_DIR}/src)
set(PROJECT_CMAKE_BIN_DIR      ${PROJECT_CMAKE_DIR}/bin)
set(PROJECT_CMAKE_BUILD_DIR    ${PROJECT_BUILD_DIR}/cmake/src)
